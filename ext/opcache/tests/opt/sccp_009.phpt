--TEST--
SCCP 009: Conditional Constant Propagation of non-escaping array elements
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
	$a[1] = 2;
	echo $a[1];
}
?>
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %ssccp_009.php:1-8
L0 (8):     RETURN int(1)

foo: ; (lines=3, args=1, vars=1, tmps=0)
    ; (after optimizer)
    ; %ssccp_009.php:2-6
L0 (2):     CV0($x) = RECV 1
L1 (5):     ECHO int(2)
L2 (6):     RETURN null
