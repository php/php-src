--TEST--
private inner class
--FILE--
<?php

class Outer {
    private class Inner {}

    private function getInner(): self:>Inner {
        return new self:>Inner();
    }

    public function getInner2(): mixed {
        return $this->getInner();
    }
}

class Foo extends Outer {
    public function getInner(): parent:>Inner {
        var_dump(parent::getInner2());
        return new parent:>Inner();
    }
}

$outer = new Foo();
var_dump($outer->getInner());
?>
--EXPECTF--
object(Outer:>Inner)#2 (0) {
}

Fatal error: Cannot access private inner class 'Outer:>Inner' in %s on line %d
