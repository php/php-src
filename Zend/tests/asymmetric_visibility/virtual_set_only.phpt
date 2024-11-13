--TEST--
Set-only virtual property must not specify asymmetric visibility
--FILE--
<?php

class Foo {
    public private(set) int $bar {
        set {}
    }
}

?>
--EXPECTF--
Fatal error: Write-only virtual property Foo::$bar must not specify asymmetric visibility in %s on line %d
