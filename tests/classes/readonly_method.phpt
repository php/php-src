--TEST--
Ensure readonly methods are not allowed
--FILE--
<?php

class Foo {
    readonly function foo() {}
}
--EXPECTF--
Fatal error: A method cannot be readonly in %s on line %d
