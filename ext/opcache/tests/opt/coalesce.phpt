--TEST--
COALESCE optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function a() {
    $test = $test ?? true;
    return $test;
}

function b() {
    $test ??= true;
    return $test;
}

?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

a:
     ; (lines=2, args=0, vars=1, tmps=1)
     ; (after optimizer)
     ; %s
0000 T1 = COALESCE CV0($test) 0001
0001 RETURN bool(true)

b:
     ; (lines=2, args=0, vars=1, tmps=1)
     ; (after optimizer)
     ; %s
0000 T1 = COALESCE CV0($test) 0001
0001 RETURN bool(true)
