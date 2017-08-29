--TEST--
DCE 005: Elimination of assignment to non-escaping objects (can't remove NEW yet)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class A {
}
function foo(int $x) {
	$a = new A;
	$a->foo = $x;
}
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %sdce_005.php:1-8
L0:     RETURN int(1)

foo: ; (lines=5, args=1, vars=1, tmps=1)
    ; (after optimizer)
    ; %sdce_005.php:4-7
L0:     CV0($x) = RECV 1
L1:     V1 = NEW 0 string("A")
L2:     DO_FCALL
L3:     FREE V1
L4:     RETURN null
