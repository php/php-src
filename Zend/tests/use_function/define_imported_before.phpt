--TEST--
using function with same name as defined should fail
--FILE--
<?php

namespace {
    function bar() {}

    use function foo\bar;
}

namespace {
    echo "Done";
}

?>
--EXPECTF--
Fatal error: Cannot import function foo\bar as bar, bar has already been declared in %s on line %d
