--TEST--
defining function with same name as imported should fail
--FILE--
<?php

namespace {
    use function foo\bar;

    function bar() {}
}

?>
--EXPECTF--
Fatal error: Function bar() has already been declared in %s on line %d
