--TEST--
PFA pipe optimization: PFA with multiple placeholders can not be optimized
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

try {
2 |> foo(a: ?, b: ?);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
$_main:
     ; (lines=19, args=0, vars=1, tmps=2)
     ; (after optimizer)
     ; %spipe_optimization_007.php:1-16
0000 INIT_FCALL 0 %d string("time")
0001 V2 = DO_ICALL
0002 T1 = IS_SMALLER int(0) V2
0003 JMPZ T1 0005
0004 DECLARE_FUNCTION string("foo") 0
0005 INIT_FCALL_BY_NAME 0 string("foo")
0006 SEND_PLACEHOLDER string("a")
0007 SEND_PLACEHOLDER string("b")
0008 T1 = CALLABLE_CONVERT_PARTIAL %d array(...)
0009 INIT_DYNAMIC_CALL 1 T1
0010 SEND_VAL_EX int(2) 1
0011 DO_FCALL
0012 RETURN int(1)
0013 CV0($e) = CATCH string("Throwable")
0014 INIT_METHOD_CALL 0 CV0($e) string("getMessage")
0015 V1 = DO_FCALL
0016 ECHO V1
0017 ECHO string("\n")
0018 RETURN int(1)
EXCEPTION TABLE:
     0005, 0013, -, -

foo:
     ; (lines=7, args=2, vars=2, tmps=0)
     ; (after optimizer)
     ; %spipe_optimization_007.php:4-6
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 INIT_FCALL 2 %d string("var_dump")
0003 SEND_VAR CV0($a) 1
0004 SEND_VAR CV1($b) 2
0005 DO_ICALL
0006 RETURN null

$_main:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s:1-10
0000 T0 = DECLARE_LAMBDA_FUNCTION 0
0001 FREE T0
0002 RETURN int(1)

{closure:%s:%d}:
     ; (lines=7, args=2, vars=2, tmps=1)
     ; (after optimizer)
     ; %s:10-10
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 INIT_FCALL 2 %d string("foo")
0003 SEND_VAR CV0($a) 1
0004 SEND_VAR CV1($b) 2
0005 V2 = DO_UCALL
0006 RETURN V2
Too few arguments to function {closure:%s:%d}(), 1 passed in %s on line %d and exactly 2 expected
