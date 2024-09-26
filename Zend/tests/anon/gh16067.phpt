--TEST--
Compiler prevents explicit `abstract` methods on anonymous classes
--FILE--
<?php

$c = new class {
    abstract public function f();
}
?>
--EXPECTF--
Fatal error: Anonymous class class@anonymous cannot contain abstract method class@anonymous::f in %s on line 4
