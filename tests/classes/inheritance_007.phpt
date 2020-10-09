--TEST--
Ensure private methods with the same name are not checked for inheritance rules - final
--FILE--
<?php
class A {
    function callYourPrivates() {
        $this->normalPrivate();
        $this->finalPrivate();
    }
    function notOverridden_callYourPrivates() {
        $this->normalPrivate();
        $this->finalPrivate();
    }
    private function normalPrivate() {
        echo __METHOD__ . PHP_EOL;
    }
    final private function finalPrivate() {
        echo __METHOD__ . PHP_EOL;
    }
}
class B extends A {
    function callYourPrivates() {
        $this->normalPrivate();
        $this->finalPrivate();
    }
    private function normalPrivate() {
        echo __METHOD__ . PHP_EOL;
    }
    final private function finalPrivate() {
        echo __METHOD__ . PHP_EOL;
    }
}
$a = new A();
$a->callYourPrivates();
$a->notOverridden_callYourPrivates();
$b = new B();
$b->callYourPrivates();
$b->notOverridden_callYourPrivates();
?>
--EXPECTF--
Warning: Private methods cannot be final as they are never overridden by other classes %s

Warning: Private methods cannot be final as they are never overridden by other classes %s
A::normalPrivate
A::finalPrivate
A::normalPrivate
A::finalPrivate
B::normalPrivate
B::finalPrivate
A::normalPrivate
A::finalPrivate
