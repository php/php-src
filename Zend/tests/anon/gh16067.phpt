--TEST--
Compiler prevents explicit `abstract` methods on anonymous classes
--FILE--
<?php

$c = new class {
    abstract public function f();
}
?>
--EXPECTF--
Fatal error: Anonymous class method f() must not be abstract in %s on line 4
