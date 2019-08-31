--TEST--
DCE 007: Escaping of enclosed arrays doesn't prevent removal of enclosing array
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
function esc($x) {
        $a = [$x];
        $b = [$a];
        return $a;
}
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %sdce_007.php:1-7
L0 (7):     RETURN int(1)

esc: ; (lines=3, args=1, vars=2, tmps=0)
    ; (after optimizer)
    ; %sdce_007.php:2-6
L0 (2):     CV0($x) = RECV 1
L1 (3):     CV1($a) = INIT_ARRAY 1 (packed) CV0($x) NEXT
L2 (5):     RETURN CV1($a)
