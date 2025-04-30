--TEST--
Get-only virtual property must not specify asymmetric visibility
--FILE--
<?php

class Foo {
    public private(set) int $bar {
        get => 42;
    }
}

?>
--EXPECTF--
Fatal error: Read-only virtual property Foo::$bar must not specify asymmetric visibility in %s on line %d
