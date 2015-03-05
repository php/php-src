--TEST--
030: Name ambiguity (import name & class name)
--FILE--
<?php
use A\B as Foo;

class Foo {
}

new Foo();
--EXPECTF--
Fatal error: Cannot declare class Foo because the name is already in use in %sns_030.php on line 4
