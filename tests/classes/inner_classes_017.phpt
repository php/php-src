--TEST--
protected return types
--FILE--
<?php

class Outer {
    protected class Inner {}

    public static function getInner(): self:>Inner {
        return new self:>Inner();
    }
}

$r = Outer::getInner();
function test($r): Outer:>Inner {
    return $r;
}
var_dump($r);
test($r);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Method getInner is public but returns a protected class: Outer:>Inner in %s:%d
Stack trace:
#0 %s(%d): Outer::getInner()
#1 {main}
  thrown in %s on line %d
