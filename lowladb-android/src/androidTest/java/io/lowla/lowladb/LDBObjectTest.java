package io.lowla.lowladb;

import junit.framework.TestCase;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

public class LDBObjectTest extends TestCase {

    public void testContainsField() throws Exception {
        LDBObject obj = new LDBObjectBuilder().appendString("myfield", "mystring").finish();
        assertTrue(obj.containsField("myfield"));
        assertFalse(obj.containsField("yourfield"));
    }

    public void testDoubleForField() throws Exception {
        LDBObject obj = new LDBObjectBuilder().appendDouble("myfield", 17.0).finish();
        assertEquals(17.0, obj.doubleForField("myfield"), 1e-10);
        assertEquals(0.0, obj.doubleForField("yourfield"), 1e-10);
    }

    public void testStringForField() throws Exception {
        LDBObject obj = new LDBObjectBuilder().appendString("myfield", "mystring").finish();
        assertEquals("mystring", obj.stringForField("myfield"));
        assertEquals("", obj.stringForField("yourfield"));
    }

    public void testObjectForField() throws Exception {
        LDBObject objSub = new LDBObjectBuilder().appendString("myfield", "mystring").finish();
        LDBObject obj = new LDBObjectBuilder().appendObject("myobject", objSub).finish();
        assertEquals(objSub, obj.objectForField("myobject"));
        assertEquals("mystring", obj.objectForField("myobject").stringForField("myfield"));
        assertNull(obj.objectForField("yourfield"));
    }

    public void testObjectIdForField() throws Exception {
        LDBObjectId oid = LDBObjectId.generate();
        LDBObject obj = new LDBObjectBuilder().appendObjectId("myfield", oid).finish();
        assertEquals(oid, obj.objectIdForField("myfield"));
        assertNull(obj.objectIdForField("yourfield"));
    }

    public void testBoolForField() throws Exception {
        LDBObject obj = new LDBObjectBuilder().appendBool("myfield", true).finish();
        assertTrue(obj.boolForField("myfield"));
        assertFalse(obj.boolForField("yourfield"));
    }

    public void testDateForField() throws Exception {
        Date date = new Date();
        LDBObject obj = new LDBObjectBuilder().appendDate("myfield", date).finish();
        assertEquals(date, obj.dateForField("myfield"));
        assertNull(obj.dateForField("yourfield"));
    }

    public void testIntForField() throws Exception {
        LDBObject obj = new LDBObjectBuilder().appendInt("myfield", 314).finish();
        assertEquals(314, obj.intForField("myfield"));
        assertEquals(0, obj.intForField("yourfield"));
        // Can't access an int as a long
        assertEquals(0, obj.longForField("myfield"));
    }

    public void testLongForField() throws Exception {
        LDBObject obj = new LDBObjectBuilder().appendLong("myfield", 3140000000L).finish();
        assertEquals(3140000000L, obj.longForField("myfield"));
        assertEquals(0, obj.longForField("yourfield"));
    }

    public void testTypeMismatch() {
        LDBObject obj = new LDBObjectBuilder().appendInt("int", 3).appendLong("notInt", 4).finish();
        assertEquals(0.0, obj.doubleForField("int"));
        assertEquals("", obj.stringForField("int"));
        assertNull(obj.objectForField("int"));
        assertNull(obj.objectIdForField("int"));
        assertFalse(obj.boolForField("int"));
        assertNull(obj.dateForField("int"));
        assertEquals(0, obj.intForField("notInt"));
        assertEquals(0, obj.longForField("int"));
    }

    public void testItCanReadObjectIdsFromAMap() {
        Map subObj = new HashMap();
        subObj.put("_bsonType", "ObjectId");
        subObj.put("hexString", "0123456789abcdef01234567");
        Map map = new HashMap();
        map.put("_id", subObj);

        LDBObject obj = LDBObject.objectWithMap(map);
        assertTrue(obj.containsField("_id"));
        LDBObjectId oid = obj.objectIdForField("_id");
        assertNotNull(oid);
        assertEquals("0123456789abcdef01234567", oid.toHexString());
    }

    public void testAsJson() {
        LDBObject obj = new LDBObjectBuilder().appendString("myfield", "mystring").finish();
        String check = obj.asJson();
        assertEquals("{\n   \"myfield\" : \"mystring\"\n}\n", check);
    }

    public void testEquals() throws Exception {
        LDBObject objA = new LDBObjectBuilder().appendInt("myfield", 1).finish();
        LDBObject objB = new LDBObjectBuilder().appendInt("myfield", 1).finish();
        LDBObject objC = new LDBObjectBuilder().appendInt("myfield", 2).finish();

        assertEquals(objA, objB);
        assertFalse(objA.equals(objC));
    }

    public void testHashCode() throws Exception {
        LDBObject objA = new LDBObjectBuilder().appendInt("myfield", 1).finish();
        LDBObject objB = new LDBObjectBuilder().appendInt("myfield", 1).finish();
        LDBObject objC = new LDBObjectBuilder().appendInt("myfield", 2).finish();

        assertEquals(objA.hashCode(), objB.hashCode());
        assertFalse(objA.hashCode() == objC.hashCode());
    }
}