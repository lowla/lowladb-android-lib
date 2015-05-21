package io.lowla.lowladb;

import android.app.Application;
import android.content.Context;
import android.test.ApplicationTestCase;

import java.util.Date;

public class LDBObjectBuilderTest extends ApplicationTestCase<Application> {

    public LDBObjectBuilderTest() {
        super(Application.class);
    }

    public void testAppendDouble() throws Exception {
        LDBObject obj = new LDBObjectBuilder().appendDouble("myfield", 3.14).finish();
        assertTrue(obj.containsField("myfield"));
        assertEquals(3.14, obj.doubleForField("myfield"));
    }

    public void testAppendString() {
        LDBObject obj = new LDBObjectBuilder().appendString("myfield", "mystring").finish();
        assertTrue(obj.containsField("myfield"));
        assertEquals("mystring", obj.stringForField("myfield"));
    }

    public void testAppendObject() {
        LDBObject subObj = new LDBObjectBuilder().appendString("myfield", "mystring").finish();
        LDBObject obj = new LDBObjectBuilder().appendObject("myfield", subObj).finish();
        assertTrue(obj.containsField("myfield"));
        assertEquals(subObj, obj.objectForField("myfield"));
    }

    public void testAppendObjectId() {
        LDBObjectId oid = LDBObjectId.generate();
        LDBObject obj = new LDBObjectBuilder().appendObjectId("myfield", oid).finish();
        assertTrue(obj.containsField("myfield"));
        assertEquals(oid, obj.objectIdForField("myfield"));
    }

    public void testAppendBool() {
        LDBObject obj = new LDBObjectBuilder().appendBool("myfield", true).finish();
        assertTrue(obj.containsField("myfield"));
        assertTrue(obj.boolForField("myfield"));
    }

    public void testAppendDate() {
        Date date = new Date();
        LDBObject obj = new LDBObjectBuilder().appendDate("myfield", date).finish();
        assertTrue(obj.containsField("myfield"));
        assertEquals(date, obj.dateForField("myfield"));
    }

    public void testAppendInt() {
        LDBObject obj = new LDBObjectBuilder().appendInt("myfield", 314).finish();
        assertTrue(obj.containsField("myfield"));
        assertEquals(314, obj.intForField("myfield"));
    }

    public void testAppendLong() {
        LDBObject obj = new LDBObjectBuilder().appendLong("myfield", 314000000000000L).finish();
        assertTrue(obj.containsField("myfield"));
        assertEquals(314000000000000L, obj.longForField("myfield"));
    }

    public void testAppendArray() {
        LDBObject obj = new LDBObjectBuilder().startArray("myarr").appendInt("0", 5).finishArray().finish();
        assertTrue(obj.containsField("myarr"));
        LDBObject arr = obj.arrayForField("myarr");
        assertEquals(5, arr.intForField("0"));
    }
}