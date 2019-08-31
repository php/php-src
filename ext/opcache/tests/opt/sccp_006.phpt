--TEST--
SCCP 006: Conditional Constant Propagation of non-escaping array elements
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
function foo(int $x) {
	$a = ["a"=>1,"a"=>2,"a"=>$x];
	echo $a["a"];
}
?>
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %ssccp_006.php:1-7
L0 (7):     RETURN int(1)

foo: ; (lines=8, args=1, vars=2, tmps=1)
    ; (after optimizer)
    ; %ssccp_006.php:2-5
L0 (2):     CV0($x) = RECV 1
L1 (3):     T2 = INIT_ARRAY 3 int(1) string("a")
L2 (3):     T2 = ADD_ARRAY_ELEMENT int(2) string("a")
L3 (3):     T2 = ADD_ARRAY_ELEMENT CV0($x) string("a")
L4 (3):     CV1($a) = QM_ASSIGN T2
L5 (4):     T2 = FETCH_DIM_R CV1($a) string("a")
L6 (4):     ECHO T2
L7 (5):     RETURN null
LIVE RANGES:
        2: L2 - L4 (tmp/var)
