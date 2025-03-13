--TEST--
scope resolution access
--FILE--
<?php

class Outer {
    public class Middle {
    }
    public static function testStatic(): self:>Middle {
        return new static:>Middle();
    }
    public static function testSelf(): self:>Middle {
        return new self:>Middle();
    }
}

class Outer2 extends Outer {
    public class Middle extends parent:>Middle {
    }

    public static function testParent(): parent:>Middle {
        return new parent:>Middle();
    }
}

var_dump(Outer::testStatic());
var_dump(Outer::testSelf());
var_dump(Outer2::testParent());
var_dump(Outer2::testStatic());
var_dump(Outer2::testSelf());

?>
--EXPECT--
object(Outer:>Middle)#1 (0) {
}
object(Outer:>Middle)#1 (0) {
}
object(Outer:>Middle)#1 (0) {
}
object(Outer2:>Middle)#1 (0) {
}
object(Outer:>Middle)#1 (0) {
}
