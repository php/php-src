--TEST--
Asymmetric visibility private(get) protected(set) not allowed
--FILE--
<?php

class Foo {
    private protected(set) string $bar;
}

?>
--EXPECTF--
Fatal error: Get visibility must be higher than set visibility in %s on line %d
