--TEST--
030: Name ambiguity (import name & class name)
--FILE--
<?php
class Foo {
}

use A::B as Foo;

new Foo();
--EXPECTF--
Fatal error: Import name 'Foo' conflicts with defined class in %sns_030.php on line 5
