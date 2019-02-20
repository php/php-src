--TEST--
SCCP 008: Conditional Constant Propagation of non-escaping array elements
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
	if ($x) {
		$a = [0,1];
	} else {
		$a = [0,2];
	}
	echo $a[1];
}
?>
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %ssccp_008.php:1-11
L0 (11):    RETURN int(1)

foo: ; (lines=8, args=1, vars=2, tmps=1)
    ; (after optimizer)
    ; %ssccp_008.php:2-9
L0 (2):     CV0($x) = RECV 1
L1 (3):     JMPZ CV0($x) L4
L2 (4):     CV1($a) = QM_ASSIGN array(...)
L3 (4):     JMP L5
L4 (6):     CV1($a) = QM_ASSIGN array(...)
L5 (8):     T2 = FETCH_DIM_R CV1($a) int(1)
L6 (8):     ECHO T2
L7 (9):     RETURN null
