package io.lowla.lowladb;

import android.app.Application;
import android.test.ApplicationTestCase;

public class LDBDatabaseTest extends ApplicationTestCase<Application> {

    public LDBDatabaseTest() {
        super(Application.class);
    }

    private LDBClient client;
    private LDBDatabase db;

    @Override
    protected void setUp() throws Exception {
        this.client = new LDBClient();
        client.dropDatabase("mydb");
        this.db = client.getDatabase("mydb");
    }

    @Override
    protected void tearDown() throws Exception {
        db = null;
        client.dropDatabase("mydb");
        client = null;
    }

    public void testCollectionNames() throws Exception {
        // No collections to start with
        String[] check = db.collectionNames();
        assertEquals(0, check.length);

        // Create a collection object - this doesn't actually create the collection yet
        LDBCollection coll = db.getCollection("coll");
        check = db.collectionNames();
        assertEquals(0, check.length);

        LDBObject object = new LDBObjectBuilder().appendString("myfield", "mystring").finish();
        coll.insert(object);
        check = db.collectionNames();
        assertEquals(1, check.length);
        assertEquals("coll", check[0]);

        // And another collection
        coll = db.getCollection("coll2.sub");
        coll.insert(object);
        check = db.collectionNames();
        assertEquals(2, check.length);
        assertEquals("coll", check[0]);
        assertEquals("coll2.sub", check[1]);
    }
}