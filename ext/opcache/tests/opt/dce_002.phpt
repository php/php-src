--TEST--
DCE 002: func_get_args() disables deletion of assignments to parameter variables
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo(int $a) {
    $a = 10;
    $b = 20;
    $x = func_get_args();
    $a = 30;
    $b = 40;
    return $x;
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sdce_002.php:1-11
0000 RETURN int(1)

foo:
     ; (lines=5, args=1, vars=2, tmps=0)
     ; (after optimizer)
     ; %sdce_002.php:2-9
0000 CV0($a) = RECV 1
0001 CV0($a) = QM_ASSIGN int(10)
0002 CV1($x) = FUNC_GET_ARGS
0003 CV0($a) = QM_ASSIGN int(30)
0004 RETURN CV1($x)
