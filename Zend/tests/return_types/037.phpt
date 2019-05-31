--TEST--
__debugInfo can only declare array as return type
--FILE--
<?php
class Foo {
    public function __debugInfo(): bool {
    }
}
?>
--EXPECTF--
Fatal error: Foo::__debugInfo(): Return type must be ?array when declared in %s on line %d
