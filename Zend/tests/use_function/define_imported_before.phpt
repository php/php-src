--TEST--
using function with same name as defined should fail
--FILE--
<?php

namespace {
    use function foo\bar;

    function bar() {}

}

namespace {
    echo "Done";
}

?>
--EXPECTF--
Fatal error: Cannot declare function bar because the name is already in use in %s on line %d
