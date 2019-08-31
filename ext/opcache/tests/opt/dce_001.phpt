--TEST--
DCE 001: Remove dead computation after constants propagation
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
function foo(string $s1, string $s2, string $s3, string $s4) {
	$x = ($s1 . $s2) . ($s3 . $s4);
	$x = 0;
	return $x;
}
?>
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %sdce_001.php:1-8
L0 (8):     RETURN int(1)

foo: ; (lines=5, args=4, vars=4, tmps=0)
    ; (after optimizer)
    ; %sdce_001.php:2-6
L0 (2):     CV0($s1) = RECV 1
L1 (2):     CV1($s2) = RECV 2
L2 (2):     CV2($s3) = RECV 3
L3 (2):     CV3($s4) = RECV 4
L4 (5):     RETURN int(0)
