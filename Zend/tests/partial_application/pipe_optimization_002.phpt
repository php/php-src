--TEST--
PFA pipe optimization: PFA with only one placeholder can be optimized
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

2 |> foo(1, ?);

?>
--EXPECTF--
$_main:
     ; (lines=10, args=0, vars=0, tmps=2)
     ; (after optimizer)
     ; %spipe_optimization_002.php:1-12
0000 INIT_FCALL 0 %d string("time")
0001 V1 = DO_ICALL
0002 T0 = IS_SMALLER int(0) V1
0003 JMPZ T0 0005
0004 DECLARE_FUNCTION string("foo") 0
0005 INIT_FCALL_BY_NAME 2 string("foo")
0006 SEND_VAL_EX int(1) 1
0007 SEND_VAL_EX int(2) 2
0008 DO_FCALL_BY_NAME
0009 RETURN int(1)

foo:
     ; (lines=7, args=2, vars=2, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_002.php:4-6
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 INIT_FCALL 2 %d string("var_dump")
0003 SEND_VAR CV0($a) 1
0004 SEND_VAR CV1($b) 2
0005 DO_ICALL
0006 RETURN null
int(1)
int(2)
