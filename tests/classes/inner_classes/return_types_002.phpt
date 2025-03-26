--TEST--
private inner class
--FILE--
<?php

class Outer {
    private class Inner {}

    private function getInner(): Inner {
        return new Inner();
    }

    public function getInner2(): mixed {
        return $this->getInner();
    }
}

class Foo extends Outer {
    public function getInner(): Outer\Inner {
        var_dump(parent::getInner2());
        return new Outer\Inner();
    }
}

$outer = new Foo();
var_dump($outer->getInner());
?>
--EXPECTF--
object(Outer\Inner)#2 (0) {
}

Fatal error: Uncaught Error: Cannot instantiate private class Outer\Inner from Foo in %s:%d
Stack trace:
#0 %s(%d): Foo->getInner()
#1 {main}
  thrown in %s on line %d
