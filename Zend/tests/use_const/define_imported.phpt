--TEST--
defining const with same name as imported should fail
--FILE--
<?php

namespace {
    use const foo\bar;

    const bar = 42;
}

?>
--EXPECTF--
Fatal error: Constant bar cannot be declared because the name is already in use in %s on line %d
