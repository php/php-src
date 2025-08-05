--TEST--
PFA pipe optimization: Evaluation order
--EXTENSIONS--
opcache
--INI--
opcache.opt_debug_level=0x20000
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache=
opcache.file_cache_only=0
--FILE--
<?php

if (time() > 0) {
    function foo($a, $b, $c) {
        var_dump($a, $b, $c);
    }
    function lhs() {
        echo __FUNCTION__, "\n";
        return 0;
    }
    function arg1() {
        echo __FUNCTION__, "\n";
        return 1;
    }
    function arg2() {
        echo __FUNCTION__, "\n";
        return 2;
    }
}

lhs() |> foo(arg1(), ?, arg2());

?>
--EXPECTF--
$_main:
     ; (lines=21, args=0, vars=0, tmps=2)
     ; (after optimizer)
     ; %spipe_optimization_009.php:1-24
0000 INIT_FCALL 0 %d string("time")
0001 V1 = DO_ICALL
0002 T0 = IS_SMALLER int(0) V1
0003 JMPZ T0 0008
0004 DECLARE_FUNCTION string("foo") 0
0005 DECLARE_FUNCTION string("lhs") 1
0006 DECLARE_FUNCTION string("arg1") 2
0007 DECLARE_FUNCTION string("arg2") 3
0008 INIT_FCALL_BY_NAME 0 string("lhs")
0009 V1 = DO_FCALL_BY_NAME
0010 T0 = QM_ASSIGN V1
0011 INIT_FCALL_BY_NAME 3 string("foo")
0012 INIT_FCALL_BY_NAME 0 string("arg1")
0013 V1 = DO_FCALL_BY_NAME
0014 SEND_VAR_NO_REF_EX V1 1
0015 SEND_VAL_EX T0 2
0016 INIT_FCALL_BY_NAME 0 string("arg2")
0017 V0 = DO_FCALL_BY_NAME
0018 SEND_VAR_NO_REF_EX V0 3
0019 DO_FCALL_BY_NAME
0020 RETURN int(1)
LIVE RANGES:
     0: 0011 - 0015 (tmp/var)

foo:
     ; (lines=9, args=3, vars=3, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_009.php:4-6
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 CV2($c) = RECV 3
0003 INIT_FCALL 3 %d string("var_dump")
0004 SEND_VAR CV0($a) 1
0005 SEND_VAR CV1($b) 2
0006 SEND_VAR CV2($c) 3
0007 DO_ICALL
0008 RETURN null

lhs:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_009.php:7-10
0000 ECHO string("lhs\n")
0001 RETURN int(0)

arg1:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_009.php:11-14
0000 ECHO string("arg1\n")
0001 RETURN int(1)

arg2:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_009.php:15-18
0000 ECHO string("arg2\n")
0001 RETURN int(2)
lhs
arg1
arg2
int(1)
int(0)
int(2)
