--TEST--
DCE 003: Assignment elimination (without FREE)
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
	$b = $a += 3;
	return $a;
}
?>
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %sdce_003.php:1-7
L0 (7):     RETURN int(1)

foo: ; (lines=3, args=1, vars=1, tmps=0)
    ; (after optimizer)
    ; %sdce_003.php:2-5
L0 (2):     CV0($a) = RECV 1
L1 (3):     CV0($a) = ADD CV0($a) int(3)
L2 (4):     RETURN CV0($a)
