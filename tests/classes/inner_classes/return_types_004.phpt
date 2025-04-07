--TEST--
private return types
--FILE--
<?php

class Outer {
    private class Inner {}

    public static function getInner(): Inner {
        return new Inner();
    }
}

$r = Outer::getInner();
function test($r): Outer\Inner {
    return $r;
}
var_dump($r);
test($r);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Public method getInner cannot return private class Outer\Inner in %s:%d
Stack trace:
#0 %s(%d): Outer::getInner()
#1 {main}
  thrown in %s on line %d
