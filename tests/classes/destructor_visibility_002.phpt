--TEST--
ZE2 Ensuring destructor visibility
--FILE--
<?php

class Base {
    private function __destruct() {
        echo __METHOD__ . "\n";
    }
}

class Derived extends Base {
}

$obj = new Derived;

?>
===DONE===
--EXPECT--
===DONE===

Warning: Call to private Derived::__destruct() from global scope during shutdown ignored in Unknown on line 0
