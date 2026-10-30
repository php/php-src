--TEST--
Using parent::$prop::get() outside a property hook
--FILE--
<?php

class A {
    public function test() {
        return parent::$prop::get();
    }
}

?>
--EXPECTF--
Fatal error: Must not use parent::$prop::get() outside a property hook in %s on line %d
