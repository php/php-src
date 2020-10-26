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
Fatal error: Cannot import class Foo\B as B, B has already been declared in %s on line %d
