--TEST--
DCE 002: func_get_args() disables deletion of assignments to parameter variables
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
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
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %sdce_002.php:1-11
L0 (11):    RETURN int(1)

foo: ; (lines=5, args=1, vars=2, tmps=0)
    ; (after optimizer)
    ; %sdce_002.php:2-9
L0 (2):     CV0($a) = RECV 1
L1 (3):     CV0($a) = QM_ASSIGN int(10)
L2 (5):     CV1($x) = FUNC_GET_ARGS
L3 (6):     CV0($a) = QM_ASSIGN int(30)
L4 (8):     RETURN CV1($x)
