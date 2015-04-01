package io.lowla.lowladb;

import android.app.Application;
import android.test.ApplicationTestCase;

import java.util.ArrayList;
import java.util.List;

import io.lowla.lowladb.platform.android.Integration;

public class LDBClientTest extends ApplicationTestCase<Application> {

    public LDBClientTest() {
        super(Application.class);
    }

    @Override
    protected void setUp() throws Exception {
        Integration.INSTANCE.setContext(getContext());
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
}