--TEST--
__unset can only declare void return
--FILE--
<?php
class Foo {
    function __unset($name) : bool {}
}
?>
--EXPECTF--
Fatal error: Foo::__unset(): Return type must be void when declared in %s on line %d
