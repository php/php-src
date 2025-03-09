--TEST--
private inner class
--FILE--
<?php

class Outer {
    private class Inner {}

    public function getInner(): self:>Inner {
        return new self:>Inner();
    }
}

class Foo extends Outer {
    public function getInner(): parent:>Inner {
        var_dump(parent::getInner());
        return new parent:>Inner();
    }
}

$outer = new Foo();
var_dump($outer->getInner());
?>
--EXPECTF--
object(Outer:>Inner)#2 (0) {
}

Fatal error: Class 'Outer:>Inner' is private in %s on line %d
