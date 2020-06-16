--TEST--
SCCP 022: Invalid types
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
    $a[0] = $x;
    $a[1] = 5;
    echo $a[1];
    $a->foo = 5;
    echo $a[1];
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_022.php:1-10
0000 RETURN int(1)

foo:
     ; (lines=10, args=1, vars=2, tmps=1)
     ; (after optimizer)
     ; %ssccp_022.php:2-8
0000 CV0($x) = RECV 1
0001 ASSIGN_DIM CV1($a) int(0)
0002 OP_DATA CV0($x)
0003 ASSIGN_DIM CV1($a) int(1)
0004 OP_DATA int(5)
0005 ECHO string("5")
0006 ASSIGN_OBJ CV1($a) string("foo")
0007 OP_DATA int(5)
0008 T2 = FETCH_DIM_R null int(1)
0009 RETURN null
