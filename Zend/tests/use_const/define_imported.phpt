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
Fatal error: Constant bar has already been declared in %s on line %d
