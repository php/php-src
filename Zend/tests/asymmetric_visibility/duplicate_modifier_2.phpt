--TEST--
Duplicate asymmetric visibility modifier
--FILE--
<?php

class Foo {
    public(set) protected(set) string $bar;
}

?>
--EXPECTF--
Fatal error: Multiple access type modifiers are not allowed in %s on line %d
