package io.lowla.lowladb;

import junit.framework.TestCase;

public class LDBCollectionTest extends TestCase {
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

    public void testItCanCreateSingleStringDocuments() {
        LDBObject object = new LDBObjectBuilder().appendString("myfield", "mystring").finish();
        LDBWriteResult wr = coll.insert(object);

        assertEquals(1, wr.getDocumentCount());
        assertNotNull(wr.getDocument(0).objectIdForField("_id"));
    }

    public void testItCreatesANewIdForEachDocument() {
        LDBObject object = new LDBObjectBuilder().appendString("myfield", "mystring").finish();
        LDBWriteResult wr = coll.insert(object);
        LDBWriteResult wr2 = coll.insert(object);
        assertFalse(wr.getDocument(0).objectIdForField("_id").equals(wr2.getDocument(0).objectIdForField("_id")));
    }

    public void testItCanFindTheFirstDocument() {
        LDBObject object = new LDBObjectBuilder().appendString("myfield", "mystring").finish();
        LDBWriteResult wr = coll.insert(object);
        LDBObject found = coll.findOne();
        String check = found.stringForField("myfield");
        LDBObjectId checkId = found.objectIdForField("_id");
        assertEquals(check, "mystring");
        assertEquals(checkId, wr.getDocument(0).objectIdForField("_id"));
    }

    public void testItCanFindTwoDocuments() {
        LDBObject object1 = new LDBObjectBuilder().appendString("myfield", "mystring1").finish();
        LDBWriteResult wr1 = coll.insert(object1);
        LDBObject object2 = new LDBObjectBuilder().appendString("myfield", "mystring2").finish();
        LDBWriteResult wr2 = coll.insert(object2);
        LDBCursor cursor = coll.find();
        assertTrue(cursor.hasNext());
        LDBObject check1 = cursor.next();
        assertTrue(cursor.hasNext());
        LDBObject check2 = cursor.next();
        assertFalse(cursor.hasNext());
        assertEquals(wr1.getDocument(0).objectIdForField("_id"), check1.objectIdForField("_id"));
        assertEquals(wr2.getDocument(0).objectIdForField("_id"), check2.objectIdForField("_id"));
    }

    public void testItCannotInsertDocumentsWithDollarFields() {
        try {
            LDBObject object = new LDBObjectBuilder().appendString("$myfield", "mystring").finish();
            coll.insert(object);
            fail("Exception expected");
        }
        catch (IllegalArgumentException e) {
        }
        catch (Throwable e) {
            fail("Unexpected exception type: " + e.toString());
        }
    }

    public void testItCannotInsertDocumentsWithDollarFieldsViaArray() {
        LDBObject object = new LDBObjectBuilder().appendString("$myfield", "mystring").finish();
        LDBObject[] arr = new LDBObject[] { object };

        try {
            coll.insertArray(arr);
            fail("Exception expected");
        }
        catch (IllegalArgumentException e) {
        }
        catch (Throwable e) {
            fail("Unexpected exception type: " + e.toString());
        }
    }

    public void testItCanRemoveADocument() {
        LDBObject object1 = new LDBObjectBuilder().appendInt("a", 1).finish();
        coll.insert(object1);
        LDBObject object2 = new LDBObjectBuilder().appendInt("a", 2).finish();
        coll.insert(object2);
        LDBObject object3 = new LDBObjectBuilder().appendInt("a", 3).finish();
        coll.insert(object3);

        LDBObject query = new LDBObjectBuilder().appendInt("a", 2).finish();
        LDBWriteResult wr = coll.remove(query);
        assertEquals(1, wr.getDocumentCount());

        LDBCursor cursor = coll.find();
        LDBObject doc = cursor.next();
        assertEquals(1, doc.intForField("a"));
        doc = cursor.next();
        assertEquals(3, doc.intForField("a"));
        doc = cursor.next();
        assertNull(doc);
    }

    public void testItCanRemoveAllDocuments() {
        LDBObject object1 = new LDBObjectBuilder().appendInt("a", 1).finish();
        coll.insert(object1);
        LDBObject object2 = new LDBObjectBuilder().appendInt("a", 2).finish();
        coll.insert(object2);
        LDBObject object3 = new LDBObjectBuilder().appendInt("a", 3).finish();
        coll.insert(object3);

        LDBWriteResult wr = coll.remove();
        assertEquals(3, wr.getDocumentCount());

        LDBCursor cursor = coll.find();
        LDBObject doc = cursor.next();
        assertNull(doc);
    }

    public void testItCanUpdateADocument() {
        LDBObject object1 = new LDBObjectBuilder().appendInt("a", 1).finish();
        coll.insert(object1);
        LDBObject object2 = new LDBObjectBuilder().appendInt("a", 2).finish();
        coll.insert(object2);

        LDBObject query = new LDBObjectBuilder().appendInt("a", 2).finish();
        LDBObject update = new LDBObjectBuilder().appendInt("a", 3).finish();

        LDBWriteResult wr = coll.update(query, update);
        assertEquals(1, wr.getDocumentCount());

        LDBCursor cursor = coll.find();
        LDBObject obj = cursor.next();
        assertEquals(1, obj.intForField("a"));
        obj = cursor.next();
        assertEquals(3, obj.intForField("a"));
        obj = cursor.next();
        assertNull(obj);
    }
}