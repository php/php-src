--TEST--
__wakeup can only declare return void
--FILE--
<?php
class Foo {
    public function __wakeup(): bool {}
}
?>
--EXPECTF--
Fatal error: Foo::__wakeup(): Return type must be void when declared in %s on line %d
