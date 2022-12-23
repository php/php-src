--TEST--
Asymmetric visibility private(set)
--FILE--
<?php

class Foo {
    private(set) string $bar = 'bar';
}

?>
--EXPECTF--
Fatal error: Property Foo::$bar with set-visibility must specify get-visibility in %s on line %d
