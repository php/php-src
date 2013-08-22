--TEST--
using function with same name as defined should fail
--FILE--
<?php

namespace {
    function bar() {}

    use function foo\bar;
}

?>
--EXPECTF--
Fatal error: Cannot use function foo\bar as bar because the name is already in use in %s on line %d
