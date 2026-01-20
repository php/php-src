--TEST--
PFA pipe optimization: PFA with unknown named parameter can be optimized
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
    function foo($a, $b = null, $c = null) {
        var_dump($a, $b, $c);
    }
}

try {
    2 |> foo(1, unknown: ?);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECTF--
$_main:
     ; (lines=11, args=0, vars=0, tmps=2)
     ; (after optimizer)
     ; %s:1-12
0000 INIT_FCALL 0 %d string("time")
0001 V1 = DO_ICALL
0002 T0 = IS_SMALLER int(0) V1
0003 JMPZ T0 0005
0004 DECLARE_FUNCTION string("foo") 0
0005 INIT_FCALL_BY_NAME 1 string("foo")
0006 SEND_VAL_EX int(1) 1
0007 SEND_VAL_EX int(2) string("unknown")
0008 CHECK_UNDEF_ARGS
0009 DO_FCALL_BY_NAME
0010 RETURN int(1)

foo:
     ; (lines=9, args=3, vars=3, tmps=0)
     ; (after optimizer)
     ; %s:4-6
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV_INIT 2 null
0002 CV2($c) = RECV_INIT 3 null
0003 INIT_FCALL 3 %d string("var_dump")
0004 SEND_VAR CV0($a) 1
0005 SEND_VAR CV1($b) 2
0006 SEND_VAR CV2($c) 3
0007 DO_ICALL
0008 RETURN null

Fatal error: Uncaught Error: Unknown named parameter $unknown in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
