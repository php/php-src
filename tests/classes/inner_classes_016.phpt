--TEST--
private return types
--FILE--
<?php

class Outer {
    private class Inner {}

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

Fatal error: Private inner class Outer:>Inner cannot be used as a type declaration in the global scope in %s on line %d
