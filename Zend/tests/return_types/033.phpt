--TEST--
__set can only declare void return
--FILE--
<?php
class Foo {
    function __set($name, $value) : string {}
}
?>
--EXPECTF--
Fatal error: Foo::__set(): Return type must be void when declared in %s on line %d
