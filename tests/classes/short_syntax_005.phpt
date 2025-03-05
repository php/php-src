--TEST--
constructor override with no params
--FILE--
<?php

class Foo() extends Exception;

throw new Foo("test");
?>
--EXPECTF--
Fatal error: Uncaught Foo: test in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
