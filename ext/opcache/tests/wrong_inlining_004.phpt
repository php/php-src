--TEST--
Inlining throgh call_user_func()
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
    $x = new stdClass;
    var_dump(call_user_func('get_const', $x));
}

test();
?>
--EXPECT--
int(42)
