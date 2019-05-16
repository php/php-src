--TEST--
Use conflicts are detected for late-bound classes
--FILE--
<?php

/* Reverse declaration order disables early-binding */
class B extends A {}
class A {}
use Foo\B;

?>
--EXPECTF--
Fatal error: Cannot use Foo\B as B because the name is already in use in %s on line %d
