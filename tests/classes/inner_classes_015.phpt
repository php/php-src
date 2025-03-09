--TEST--
protected inner class
--FILE--
<?php

class Outer {
    protected class Inner {}

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
var_dump(new Outer:>Inner());
?>
--EXPECTF--
object(Outer:>Inner)#2 (0) {
}
object(Outer:>Inner)#2 (0) {
}

Fatal error: Class 'Outer:>Inner' is protected in %s on line %d
