--TEST--
SCCP 024: Const replacing to op2 of INSTANCEOF
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
class A {
    function t($obj) {
        $a = "A";
        $b = "self";
        $c = 1;
        echo ($obj instanceof $a);
        echo ($obj instanceof $b);
        echo ($obj instanceof $c);
    }
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_024.php:1-13
0000 RETURN int(1)

A::t:
     ; (lines=10, args=1, vars=2, tmps=2)
     ; (after optimizer)
     ; %ssccp_024.php:3-10
0000 CV0($obj) = RECV 1
0001 CV1($c) = QM_ASSIGN int(1)
0002 T2 = INSTANCEOF CV0($obj) string("A")
0003 ECHO T2
0004 T2 = INSTANCEOF CV0($obj) string("self")
0005 ECHO T2
0006 V3 = FETCH_CLASS (no-autolod) (exception) CV1($c)
0007 T2 = INSTANCEOF CV0($obj) V3
0008 ECHO T2
0009 RETURN null
