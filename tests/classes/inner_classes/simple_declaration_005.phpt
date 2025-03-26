--TEST--
failed inheritance
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
    public class Middle {
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
var_dump(Outer2::testSelf());

?>
--EXPECTF--
Fatal error: Declaration of Outer2::testSelf(): Outer2\Middle must be compatible with Outer::testSelf(): Outer\Middle in %s on line %d
