--TEST--
array_map(): foreach optimization - const array
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

function plus1($x) {
    return $x + 1;
}

$foo = array_map(plus1(...), [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]);

var_dump($foo);

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 TYPE_ASSERT 131079 string("array_map") array(...)
0001 V1 = INIT_ARRAY 0 (packed) NEXT
0002 V2 = FE_RESET_R array(...) 0009
0003 T4 = FE_FETCH_R V2 T3 0009
0004 INIT_FCALL 1 %d string("plus1")
0005 SEND_VAL T3 1
0006 V3 = DO_UCALL
0007 V1 = ADD_ARRAY_ELEMENT V3 T4
0008 JMP 0003
0009 FE_FREE V2
0010 ASSIGN CV0($foo) V1
0011 INIT_FCALL 1 %d string("var_dump")
0012 SEND_VAR CV0($foo) 1
0013 DO_ICALL
0014 RETURN int(1)
LIVE RANGES:
     1: 0002 - 0010 (tmp/var)
     2: 0003 - 0009 (loop)

plus1:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %s
0000 CV0($x) = RECV 1
0001 T1 = ADD CV0($x) int(1)
0002 RETURN T1
array(10) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
  [4]=>
  int(6)
  [5]=>
  int(7)
  [6]=>
  int(8)
  [7]=>
  int(9)
  [8]=>
  int(10)
  [9]=>
  int(11)
}
