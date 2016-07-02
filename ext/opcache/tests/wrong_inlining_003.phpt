--TEST--
foo($bar) with undefined $bar
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function get_const() {
    return 42;
}

function test() {
    var_dump(get_const($undef));
}

test();
?>
--EXPECTF--
Notice: Undefined variable: undef in %swrong_inlining_003.php on line 7
int(42)
