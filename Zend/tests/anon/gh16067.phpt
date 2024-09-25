--TEST--
Compiler prevents explicit `abstract` methods on anonymous classes
--FILE--
<?php

$c = new class {
    abstract public function f();
}
?>
--EXPECTF--
Fatal error: Class class@anonymous must implement 1 abstract method (class@anonymous::f) in %s on line 3
