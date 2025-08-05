--TEST--
PFA pipe optimization: References
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
    function foo(&$a, $b) {
        var_dump($a, $b);
        $a = 2;
    }
}

1 |> foo($a, ?);
var_dump($a);

?>
--EXPECTF--
$_main:
     ; (lines=13, args=0, vars=1, tmps=2)
     ; (after optimizer)
     ; %spipe_optimization_010.php:1-14
0000 INIT_FCALL 0 %d string("time")
0001 V2 = DO_ICALL
0002 T1 = IS_SMALLER int(0) V2
0003 JMPZ T1 0005
0004 DECLARE_FUNCTION string("foo") 0
0005 INIT_FCALL_BY_NAME 2 string("foo")
0006 SEND_VAR_EX CV0($a) 1
0007 SEND_VAL_EX int(1) 2
0008 DO_FCALL_BY_NAME
0009 INIT_FCALL 1 %d string("var_dump")
0010 SEND_VAR CV0($a) 1
0011 DO_ICALL
0012 RETURN int(1)

foo:
     ; (lines=8, args=2, vars=2, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_010.php:4-7
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 INIT_FCALL 2 %d string("var_dump")
0003 SEND_VAR CV0($a) 1
0004 SEND_VAR CV1($b) 2
0005 DO_ICALL
0006 ASSIGN CV0($a) int(2)
0007 RETURN null
NULL
int(1)
int(2)
