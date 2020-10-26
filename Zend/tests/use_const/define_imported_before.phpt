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
Fatal error: Cannot import constant foo\bar as bar, bar has already been declared in %s on line %d
