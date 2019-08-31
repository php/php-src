--TEST--
DCE 008: Escaping of enclosed arrays doesn't prevent removal of enclosing array
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
function esc(int $x) {
        $a[0] = $x;
        $b[0] = $a;
        return $a;
}
--EXPECTF--
$_main: ; (lines=1, args=0, vars=0, tmps=0)
    ; (after optimizer)
    ; %sdce_008.php:1-7
L0 (7):     RETURN int(1)

esc: ; (lines=4, args=1, vars=2, tmps=0)
    ; (after optimizer)
    ; %sdce_008.php:2-6
L0 (2):     CV0($x) = RECV 1
L1 (3):     ASSIGN_DIM CV1($a) int(0)
L2 (3):     OP_DATA CV0($x)
L3 (5):     RETURN CV1($a)
