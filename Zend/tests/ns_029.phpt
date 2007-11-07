--TEST--
029: Name ambiguity (class name & import name)
--FILE--
<?php
use A::B as Foo;

class Foo {
}

new Foo();
--EXPECTF--
Fatal error: Class name 'Foo' conflicts with import name in %sns_029.php on line 4
