package io.lowla.lowladb;

import android.app.Application;
import android.test.ApplicationTestCase;

import com.squareup.okhttp.mockwebserver.MockResponse;
import com.squareup.okhttp.mockwebserver.MockWebServer;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.Iterator;
import java.util.List;

import io.lowla.lowladb.platform.android.Integration;

public class LDBClientTest extends ApplicationTestCase<Application> {

    public LDBClientTest() {
        super(Application.class);
    }

    @Override
    protected void setUp() throws Exception {
        Integration.INSTANCE.init(getContext(), "test");
        Integration.INSTANCE.setProperty("sequence", null);
    }

    public void testGetVersion() throws Exception {
        System.loadLibrary("lowladbjni");
        assertEquals("0.0.1 (liblowladb 0.0.2)", LDBClient.getVersion());
    }

    public void testItCanCreateDatabaseReferences()  {
        LDBClient client = new LDBClient();
        LDBDatabase db = client.getDatabase("mydb");
        assertEquals("mydb", db.getName());
    }

    public void testItCanCreateCollectionReferences() {
        LDBClient client = new LDBClient();
        LDBDatabase db = client.getDatabase("mydb");
        LDBCollection coll = db.getCollection("mycoll.dotted");
        assertEquals(db, coll.getDatabase());
        assertEquals("mycoll.dotted", coll.getName());
    }

    public void testCollectionListeners() {
        class CheckListener extends LDBClient.CollectionChangedListener {
            @Override
            public void onCollectionChanged(String ns) {
                calls.add(ns);
            }

            public List<String> getCalls() {
                return calls;
            }

            private List<String> calls = new ArrayList<String>();
        }

        CheckListener l = new CheckListener();
        try {
            LDBClient.addCollectionChangedListener(l);

            LDBClient client = new LDBClient();
            LDBDatabase db = client.getDatabase("mydb");
            LDBCollection coll1 = db.getCollection("mycoll");
            LDBCollection coll2 = db.getCollection("mycoll2");

            assertEquals(0, l.getCalls().size());
            LDBClient.enableNotifications(true);
            coll1.insert(new LDBObjectBuilder().appendInt("a", 2).finish());
            assertEquals(1, l.getCalls().size());
            assertEquals("mydb.mycoll", l.getCalls().get(0));
            coll1.insert(new LDBObjectBuilder().appendInt("a", 2).finish());
            assertEquals(2, l.getCalls().size());
            assertEquals("mydb.mycoll", l.getCalls().get(1));
            LDBClient.enableNotifications(false);
            coll1.insert(new LDBObjectBuilder().appendInt("a", 2).finish());
            assertEquals(2, l.getCalls().size());
            LDBClient.enableNotifications(true);
            coll2.insert(new LDBObjectBuilder().appendInt("a", 2).finish());
            assertEquals(3, l.getCalls().size());
            assertEquals("mydb.mycoll2", l.getCalls().get(2));
        }
        finally {
            LDBClient.enableNotifications(false);
        }
    }

    public void testSyncBadServer() {
        final boolean[] check = new boolean[1];

        LDBClient.sync("http://notaserver.teamstudio.com", new LDBClient.SyncNotifier() {
            @Override
            public void notify(LDBClient.LDBSyncStatus status, String message) {
                if (LDBClient.LDBSyncStatus.ERROR == status) {
                    check[0] = true;
                }
            }
        });
        assertTrue(check[0]);
    }

    public void testNoOpSync() throws IOException {
        final boolean[] check = new boolean[1];
        final MockWebServer server = new MockWebServer();
        final List events = new ArrayList<LDBClient.LDBSyncStatus>();

        clearDataDirectory();

        server.enqueue(new MockResponse().setBody("{ \"atoms\" : [], \"sequence\" : 2}"));
        server.start();
        URL url = server.getUrl("");
        LDBClient.sync(url.toString(), new LDBClient.SyncNotifier() {
            @Override
            public void notify(LDBClient.LDBSyncStatus status, String message) {
                events.add(status);
                if (LDBClient.LDBSyncStatus.ERROR == status || LDBClient.LDBSyncStatus.OK == status) {
                    check[0] = true;
                    // No pushes, no pulls, just the changes request
                    try {
                        assertEquals(1, server.getRequestCount());
                        assertEquals("/_lowla/changes?seq=0", server.takeRequest().getPath());
                        assertEquals("2", Integration.INSTANCE.getProperty("sequence", ""));
                    } catch (InterruptedException e) {
                    }
                }
            }
        });

        server.shutdown();
        assertEquals(5, events.size());
        assertEquals(LDBClient.LDBSyncStatus.PUSH_STARTED, events.get(0));
        assertEquals(LDBClient.LDBSyncStatus.PUSH_ENDED, events.get(1));
        assertEquals(LDBClient.LDBSyncStatus.PULL_STARTED, events.get(2));
        assertEquals(LDBClient.LDBSyncStatus.PULL_ENDED, events.get(3));
        assertEquals(LDBClient.LDBSyncStatus.OK, events.get(4));
        assertTrue(check[0]);
    }

    public void testOnePullSync() throws IOException {
        final boolean[] check = new boolean[1];
        final MockWebServer server = new MockWebServer();
        final List events = new ArrayList<LDBClient.LDBSyncStatus>();

        server.enqueue(new MockResponse().setBody("{ \"atoms\" : [{ \"id\" : \"mydb.mycoll$1\", \"sequence\" : 1, \"version\" : 1, \"deleted\" : false, \"clientNs\" : \"mydb.mycoll\" }], \"sequence\" : 2}"));
        server.enqueue(new MockResponse().setBody("[{ \"id\" : \"mydb.mycoll$1\", \"clientNs\" : \"mydb.mycoll\" }, { \"_id\" : \"1\", \"myfield\" : \"myvalue\" }]"));
        server.enqueue(new MockResponse().setBody("{ \"atoms\" : [], \"sequence\" : 2}"));
        server.start();
        URL url = server.getUrl("");
        LDBClient.sync(url.toString(), new LDBClient.SyncNotifier() {
            @Override
            public void notify(LDBClient.LDBSyncStatus status, String message) {
                if (LDBClient.LDBSyncStatus.ERROR == status || LDBClient.LDBSyncStatus.OK == status) {
                    assertEquals(LDBClient.LDBSyncStatus.OK, status);
                    assertEquals("2", Integration.INSTANCE.getProperty("sequence", "0"));
                }
            }
        });

        // And sync again to make sure we don't try to push up the new record
        LDBClient.sync(url.toString(), new LDBClient.SyncNotifier() {
            @Override
            public void notify(LDBClient.LDBSyncStatus status, String message) {
                if (LDBClient.LDBSyncStatus.ERROR == status || LDBClient.LDBSyncStatus.OK == status) {
                    assertEquals(LDBClient.LDBSyncStatus.OK, status);
                }
            }
        });

        // The first sync has two requests: changes and pull
        // The second sync has one: changes
        try {
            assertEquals(3, server.getRequestCount());
            assertEquals("/_lowla/changes?seq=0", server.takeRequest().getPath());
            assertEquals("/_lowla/pull", server.takeRequest().getPath());
            assertEquals("/_lowla/changes?seq=2", server.takeRequest().getPath());
            assertEquals("2", Integration.INSTANCE.getProperty("sequence", "0"));
        }
        catch (InterruptedException offs) {}
        server.shutdown();
    }

    private void clearDataDirectory() {
        File dir = new File(Integration.INSTANCE.getDataDirectory());
        for (File child : dir.listFiles()) {
            child.delete();
        }
    }
}