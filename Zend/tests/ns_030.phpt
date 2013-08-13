--TEST--
030: Name ambiguity (import name & class name)
--FILE--
<?php
class Foo {
}

use A\B as Foo;

new Foo();
--EXPECTF--
Fatal error: Cannot use A\B as Foo because the name is already in use in %sns_030.php on line 5
