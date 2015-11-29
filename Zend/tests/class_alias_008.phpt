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
Fatal error: Uncaught Error: Cannot instantiate abstract class foo in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
