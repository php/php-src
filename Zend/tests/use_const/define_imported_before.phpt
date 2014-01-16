--TEST--
using const with same name as defined should fail
--FILE--
<?php

namespace {
    const bar = 42;

    use const foo\bar;
}

namespace {
    echo "Done";
}

?>
--EXPECTF--
Fatal error: Cannot use const foo\bar as bar because the name is already in use in %s on line %d
