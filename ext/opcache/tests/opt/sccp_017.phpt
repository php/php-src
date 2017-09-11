--TEST--
SCCP 017: Array assignemnt
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo() {
	$a = [];
	$b = $a;
	$a[0] = 5;
	$b[0] = 42;
	return $a[0];
}
?>
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %ssccp_017.php:1-10
L0:     RETURN int(1)

foo: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %ssccp_017.php:2-8
L0:     RETURN int(5)
