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
Fatal error: Function bar() cannot be declared because the name is already in use in %s on line %d
