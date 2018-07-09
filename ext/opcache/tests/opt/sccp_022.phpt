--TEST--
SCCP 022: Invailid types
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo(int $x) {
	$a[0] = $x;
	$a[1] = 5;
	echo $a[1];
	$a->foo = 5;
	echo $a[1];
}
?>
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %ssccp_022.php:1-10
L0 (10):    RETURN int(1)

foo: ; (lines=11, args=1, vars=2, tmps=1)
    ; (after optimizer)
    ; %ssccp_022.php:2-8
L0 (2):     CV0($x) = RECV 1
L1 (3):     ASSIGN_DIM CV1($a) int(0)
L2 (3):     OP_DATA CV0($x)
L3 (4):     ASSIGN_DIM CV1($a) int(1)
L4 (4):     OP_DATA int(5)
L5 (5):     ECHO int(5)
L6 (6):     ASSIGN_OBJ CV1($a) string("foo")
L7 (6):     OP_DATA int(5)
L8 (7):     T2 = FETCH_DIM_R CV1($a) int(1)
L9 (7):     ECHO T2
L10 (8):    RETURN null
