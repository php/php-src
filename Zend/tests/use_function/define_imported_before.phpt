--TEST--
using function with same name as defined should fail
--FILE--
<?php

namespace {
    function bar() {}

    use function foo\bar;

    bar();
}

?>
--EXPECTF--
Fatal error: Call to undefined function foo\bar() in %s on line %d
