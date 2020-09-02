--TEST--
DCE 004: Elimination of assignment to non-escaping arrays
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
function foo(int $x, int $y) {
    $a = [$x];
    $a[1] = $y;
    $a = $y;
    return $a;
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sdce_004.php:1-9
0000 RETURN int(1)

foo:
     ; (lines=4, args=2, vars=3, tmps=0)
     ; (after optimizer)
     ; %sdce_004.php:2-7
0000 CV0($x) = RECV 1
0001 CV1($y) = RECV 2
0002 CV2($a) = QM_ASSIGN CV1($y)
0003 RETURN CV2($a)
