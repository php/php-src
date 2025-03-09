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
object(Outer:>Inner)#1 (0) {
}

Fatal error: Protected inner class Outer:>Inner cannot be used as a type declaration in the global scope in %s on line %d
