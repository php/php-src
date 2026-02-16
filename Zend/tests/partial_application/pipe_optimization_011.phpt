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
    function arg1() {
        global $a;
        $a = 2;
        echo __FUNCTION__, "\n";
        return 1;
    }
    function arg2() {
        global $a;
        $a = 3;
        echo __FUNCTION__, "\n";
        return 2;
    }
}

$a = 0;
$a |> foo(arg1(), ?, arg2());

?>
--EXPECTF--
$_main:
     ; (lines=19, args=0, vars=1, tmps=2)
     ; (after optimizer)
     ; %spipe_optimization_011.php:1-25
0000 INIT_FCALL 0 %d string("time")
0001 T2 = DO_ICALL
0002 T1 = IS_SMALLER int(0) T2
0003 JMPZ T1 0007
0004 DECLARE_FUNCTION string("foo") 0
0005 DECLARE_FUNCTION string("arg1") %d
0006 DECLARE_FUNCTION string("arg2") %d
0007 ASSIGN CV0($a) int(0)
0008 T1 = QM_ASSIGN CV0($a)
0009 INIT_FCALL_BY_NAME 3 string("foo")
0010 INIT_FCALL_BY_NAME 0 string("arg1")
0011 V2 = DO_FCALL_BY_NAME
0012 SEND_VAR_NO_REF_EX V2 1
0013 SEND_VAL_EX T1 2
0014 INIT_FCALL_BY_NAME 0 string("arg2")
0015 V1 = DO_FCALL_BY_NAME
0016 SEND_VAR_NO_REF_EX V1 3
0017 DO_FCALL_BY_NAME
0018 RETURN int(1)
LIVE RANGES:
     1: 0009 - 0013 (tmp/var)

foo:
     ; (lines=9, args=3, vars=3, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_011.php:4-6
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 CV2($c) = RECV 3
0003 INIT_FCALL 3 %d string("var_dump")
0004 SEND_VAR CV0($a) 1
0005 SEND_VAR CV1($b) 2
0006 SEND_VAR CV2($c) 3
0007 DO_ICALL
0008 RETURN null

arg1:
     ; (lines=4, args=0, vars=1, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_011.php:7-12
0000 BIND_GLOBAL CV0($a) string("a")
0001 ASSIGN CV0($a) int(2)
0002 ECHO string("arg1\n")
0003 RETURN int(1)

arg2:
     ; (lines=4, args=0, vars=1, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_011.php:13-18
0000 BIND_GLOBAL CV0($a) string("a")
0001 ASSIGN CV0($a) int(3)
0002 ECHO string("arg2\n")
0003 RETURN int(2)
arg1
arg2
int(1)
int(0)
int(2)
