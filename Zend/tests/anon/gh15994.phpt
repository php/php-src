--TEST--
Abstract function must be implemented
--FILE--
<?php

$c = new class {
    abstract public function f();
};
?>
--EXPECTF--
Fatal error: Class class@anonymous must implement 1 abstract method (class@anonymous::f) in %sgh15994.php on line 3
