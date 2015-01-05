--TEST--
Testing class_alias() with abstract class using an arbitrary class name as alias
--FILE--
<?php

abstract class foo { }

class_alias('foo', "\0");

$a = "\0";

new $a;

?>
--EXPECTF--
Fatal error: Cannot instantiate abstract class foo in %s on line %d
