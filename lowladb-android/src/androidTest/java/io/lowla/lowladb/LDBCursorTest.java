package io.lowla.lowladb;

import junit.framework.TestCase;

public class LDBCursorTest extends TestCase {
    private LDBClient client;
    private LDBDatabase db;
    private LDBCollection coll;

    @Override
    protected void setUp() throws Exception {
        this.client = new LDBClient();
        client.dropDatabase("mydb");
        this.db = client.getDatabase("mydb");
        this.coll = db.getCollection("mycoll");
    }

    @Override
    protected void tearDown() throws Exception {
        coll = null;
        db = null;
        client.dropDatabase("mydb");
        client = null;
    }

    public void testCount() {
        coll.insert(new LDBObjectBuilder().appendInt("a", 1).finish());
        coll.insert(new LDBObjectBuilder().appendInt("a", 2).finish());
        coll.insert(new LDBObjectBuilder().appendInt("a", 3).finish());

        assertEquals(3, coll.find().count());

        assertEquals(1, coll.find(new LDBObjectBuilder().appendInt("a", 2).finish()).count());
        assertEquals(2, coll.find().limit(2).count());
        assertEquals(3, coll.find().limit(20).count());
    }

    public void testSort() {
        coll.insert(new LDBObjectBuilder().appendInt("a", 1).appendInt("b", 1).finish());
        coll.insert(new LDBObjectBuilder().appendInt("a", 2).appendInt("b", 20).finish());
        coll.insert(new LDBObjectBuilder().appendInt("a", 2).appendInt("b", 30).finish());

        LDBObject sort = new LDBObjectBuilder().appendInt("a", 1).appendInt("b", -1).finish();

        LDBCursor cursor = coll.find().sort(sort);

        LDBObject doc = cursor.next();
        assertEquals(1, doc.intForField("a"));
        doc = cursor.next();
        assertEquals(2, doc.intForField("a"));
        assertEquals(30, doc.intForField("b"));
        doc = cursor.next();
        assertEquals(2, doc.intForField("a"));
        assertEquals(20, doc.intForField("b"));
        doc = cursor.next();
        assertNull(doc);
    }
}