--TEST--
030: Name ambiguity (import name & class name)
--FILE--
<?php
class Foo {
}

import A::B as Foo;

new Foo();
--EXPECTF--
Fatal error: Import name 'Foo' coflicts with defined class in %sns_030.php on line 5
