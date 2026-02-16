--TEST--
array_map(): foreach optimization - unoptimizable PFA
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

function plusn($x, $n) {
    return $x + $n;
}

$array = range(1, 10);

$foo = array_map(plusn(n: 2, ...), $array);

var_dump($foo);

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 2 %d string("range")
0001 SEND_VAL int(1) 1
0002 SEND_VAL int(10) 2
0003 V2 = DO_ICALL
0004 ASSIGN CV0($array) V2
0005 INIT_FCALL 2 %d string("array_map")
0006 INIT_FCALL 0 %d string("plusn")
0007 SEND_VAL int(2) string("n")
0008 T2 = CALLABLE_CONVERT_PARTIAL 2
0009 SEND_VAL T2 1
0010 SEND_VAR CV0($array) 2
0011 V2 = DO_ICALL
0012 ASSIGN CV1($foo) V2
0013 INIT_FCALL 1 %d string("var_dump")
0014 SEND_VAR CV1($foo) 1
0015 DO_ICALL
0016 RETURN int(1)

plusn:
     ; (lines=4, args=2, vars=2, tmps=1)
     ; (after optimizer)
     ; %s
0000 CV0($x) = RECV 1
0001 CV1($n) = RECV 2
0002 T2 = ADD CV0($x) CV1($n)
0003 RETURN T2

$_main:
     ; (lines=4, args=0, vars=1, tmps=1)
     ; (after optimizer)
     ; %s:1-9
0000 T1 = DECLARE_LAMBDA_FUNCTION 0
0001 BIND_LEXICAL T1 CV0($n)
0002 FREE T1
0003 RETURN int(1)
LIVE RANGES:
     1: 0001 - 0002 (tmp/var)

{closure:pfa:%s:9}:
     ; (lines=18, args=1, vars=2, tmps=2)
     ; (after optimizer)
     ; %s:9-9
0000 CV0($x) = RECV 1
0001 BIND_STATIC CV1($n)
0002 T3 = FUNC_NUM_ARGS
0003 T2 = IS_SMALLER_OR_EQUAL T3 int(1)
0004 JMPZ T2 0010
0005 INIT_FCALL 2 %d string("plusn")
0006 SEND_VAR CV0($x) 1
0007 SEND_VAR CV1($n) 2
0008 V2 = DO_UCALL
0009 RETURN V2
0010 INIT_FCALL 2 %d string("plusn")
0011 SEND_VAR CV0($x) 1
0012 SEND_VAR CV1($n) 2
0013 T2 = FUNC_GET_ARGS int(1)
0014 SEND_UNPACK T2
0015 CHECK_UNDEF_ARGS
0016 V2 = DO_UCALL
0017 RETURN V2
array(10) {
  [0]=>
  int(3)
  [1]=>
  int(4)
  [2]=>
  int(5)
  [3]=>
  int(6)
  [4]=>
  int(7)
  [5]=>
  int(8)
  [6]=>
  int(9)
  [7]=>
  int(10)
  [8]=>
  int(11)
  [9]=>
  int(12)
}
