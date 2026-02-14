--TEST--
PFA optimization: PFA with single placeholder arg can be optimized
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
    function foo($a) {
        var_dump($a);
    }
}

1 |> foo(?);

?>
--EXPECTF--
$_main:
     ; (lines=9, args=0, vars=0, tmps=2)
     ; (after optimizer)
     ; %spipe_optimization_001.php:1-12
0000 INIT_FCALL 0 %d string("time")
0001 T1 = DO_ICALL
0002 T0 = IS_SMALLER int(0) T1
0003 JMPZ T0 0005
0004 DECLARE_FUNCTION string("foo") 0
0005 INIT_FCALL_BY_NAME 1 string("foo")
0006 SEND_VAL_EX int(1) 1
0007 DO_FCALL_BY_NAME
0008 RETURN int(1)

foo:
     ; (lines=5, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_001.php:4-6
0000 CV0($a) = RECV 1
0001 INIT_FCALL 1 %d string("var_dump")
0002 SEND_VAR CV0($a) 1
0003 DO_ICALL
0004 RETURN null
int(1)
