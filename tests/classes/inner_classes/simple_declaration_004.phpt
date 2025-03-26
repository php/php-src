--TEST--
scope resolution access
--FILE--
<?php

class Outer {
    public class Middle {
    }
    public static function testSelf(): Middle {
        return new Middle();
    }
}

class Outer2 extends Outer {
    public class Middle extends Outer\Middle {
    }

    public static function testParent(): Outer\Middle {
        return new Outer\Middle();
    }

    public static function testSelf(): Middle {
        return new Middle();
    }
}

var_dump(Outer::testSelf());
var_dump(Outer2::testParent());
var_dump(Outer2::testSelf());

?>
--EXPECT--
object(Outer\Middle)#1 (0) {
}
object(Outer\Middle)#1 (0) {
}
object(Outer2\Middle)#1 (0) {
}
