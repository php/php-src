--TEST--
__unserialize can only declare void return
--FILE--
<?php
class Foo {
    public function __unserialize(array $data): array {}
}
?>
--EXPECTF--
Fatal error: Foo::__unserialize(): Return type must be void when declared in %s on line %d
