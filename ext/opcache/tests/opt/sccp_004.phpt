--TEST--
SCCP 004: Conditional Constant Propagation of non-escaping array elements
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
	$a = [1,2,3];
	$a[2] = $x;
	$i = 1;
	$c = $i < 2;
	if ($c) {
		$k = 2 * $i;
		$a[$k] = $i;
//		$a[$k]++;
		echo isset($a[$k]);
//		$a[$k] += 5;
	}
	echo $a[2];
}
?>
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %ssccp_004.php:1-17
L0 (17):    RETURN int(1)

foo: ; (lines=4, args=1, vars=1, tmps=0)
    ; (after optimizer)
    ; %ssccp_004.php:2-15
L0 (2):     CV0($x) = RECV 1
L1 (11):    ECHO bool(true)
L2 (14):    ECHO int(1)
L3 (15):    RETURN null
