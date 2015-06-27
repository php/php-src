--TEST--
using const with same name as defined should fail
--FILE--
<?php

namespace {
    const bar = 42;

    use const foo\bar;

    echo bar;
}

?>
--EXPECTF--
Fatal error: Undefined constant 'foo\bar' in %s on line %d
