--TEST--
SCCP 019: Array assignment
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
function foo(int $x) {
    $a[0] = 5;
    $a[1] = $x;
    $b = $a;
    return $b[0];
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_019.php:1-9
0000 RETURN int(1)

foo:
     ; (lines=2, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %ssccp_019.php:2-7
0000 CV0($x) = RECV 1
0001 RETURN int(5)
