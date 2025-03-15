--TEST--
protected inner class
--FILE--
<?php

class Outer {
    protected class Inner {}

    protected function getInner(): self:>Inner {
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

Fatal error: Uncaught TypeError: Public method getInner cannot return protected class Outer:>Inner in %s:%d
Stack trace:
#0 %s(%d): Foo->getInner()
#1 {main}
  thrown in %s on line %d
