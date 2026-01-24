--TEST--
PFA optimization: PFA with named args and a variadic placeholder can not be optimized
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
    function foo($a, $b) {
        var_dump($a, $b);
    }
}

1 |> foo(b: 2, ...);

?>
--EXPECTF--
$_main:
     ; (lines=12, args=0, vars=0, tmps=2)
     ; (after optimizer)
     ; %s:1-12
0000 INIT_FCALL 0 %d string("time")
0001 V1 = DO_ICALL
0002 T0 = IS_SMALLER int(0) V1
0003 JMPZ T0 0005
0004 DECLARE_FUNCTION string("foo") 0
0005 INIT_FCALL_BY_NAME 0 string("foo")
0006 SEND_VAL_EX int(2) string("b")
0007 T0 = CALLABLE_CONVERT_PARTIAL 3
0008 INIT_DYNAMIC_CALL 1 T0
0009 SEND_VAL_EX int(1) 1
0010 DO_FCALL
0011 RETURN int(1)

foo:
     ; (lines=7, args=2, vars=2, tmps=0)
     ; (after optimizer)
     ; %s:4-6
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 INIT_FCALL 2 %d string("var_dump")
0003 SEND_VAR CV0($a) 1
0004 SEND_VAR CV1($b) 2
0005 DO_ICALL
0006 RETURN null

$_main:
     ; (lines=4, args=0, vars=1, tmps=1)
     ; (after optimizer)
     ; %s:1-9
0000 T1 = DECLARE_LAMBDA_FUNCTION 0
0001 BIND_LEXICAL T1 CV0($b)
0002 FREE T1
0003 RETURN int(1)
LIVE RANGES:
     1: 0001 - 0002 (tmp/var)

{closure:pfa:%s:9}:
     ; (lines=18, args=1, vars=2, tmps=2)
     ; (after optimizer)
     ; %s:9-9
0000 CV0($a) = RECV 1
0001 BIND_STATIC CV1($b)
0002 T3 = FUNC_NUM_ARGS
0003 T2 = IS_SMALLER_OR_EQUAL T3 int(1)
0004 JMPZ T2 0010
0005 INIT_FCALL 2 %d string("foo")
0006 SEND_VAR CV0($a) 1
0007 SEND_VAR CV1($b) 2
0008 V2 = DO_UCALL
0009 RETURN V2
0010 INIT_FCALL 2 %d string("foo")
0011 SEND_VAR CV0($a) 1
0012 SEND_VAR CV1($b) 2
0013 T2 = FUNC_GET_ARGS int(1)
0014 SEND_UNPACK T2
0015 CHECK_UNDEF_ARGS
0016 V2 = DO_UCALL
0017 RETURN V2
int(1)
int(2)
