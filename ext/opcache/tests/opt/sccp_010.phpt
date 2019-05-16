--TEST--
SCCP 010: Conditional Constant Propagation of non-escaping object properties
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
function foo() {
    $o = new stdClass();
    $o->foo = 0;
	$i = 1;
	$c = $i < 2;
	if ($c) {
		$k = 2 * $i;
		$o->foo = $i;
		echo $o->foo;
	}
	echo $o->foo;
}
?>
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %ssccp_010.php:1-15
L0 (15):    RETURN int(1)

foo: ; (lines=3, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %ssccp_010.php:2-13
L0 (10):    ECHO int(1)
L1 (12):    ECHO int(1)
L2 (13):    RETURN null
