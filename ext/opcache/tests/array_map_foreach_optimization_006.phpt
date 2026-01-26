--TEST--
array_map(): foreach optimization - dynamic name
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

$array = range(1, 10);

$plus1 = 'plus1';
$foo = array_map($plus1(...), $array);

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
0003 V3 = DO_ICALL
0004 ASSIGN CV0($array) V3
0005 ASSIGN CV1($plus1) string("plus1")
0006 TYPE_ASSERT 131079 string("array_map") CV0($array)
0007 T3 = INIT_ARRAY 0 (packed) NEXT
0008 V4 = FE_RESET_R CV0($array) 0015
0009 T6 = FE_FETCH_R V4 T5 0015
0010 INIT_DYNAMIC_CALL 1 CV1($plus1)
0011 SEND_VAL_EX T5 1
0012 V5 = DO_FCALL
0013 T3 = ADD_ARRAY_ELEMENT V5 T6
0014 JMP 0009
0015 FE_FREE V4
0016 ASSIGN CV2($foo) T3
0017 INIT_FCALL 1 %d string("var_dump")
0018 SEND_VAR CV2($foo) 1
0019 DO_ICALL
0020 RETURN int(1)
LIVE RANGES:
     3: 0008 - 0016 (tmp/var)
     4: 0009 - 0015 (loop)
     5: 0010 - 0011 (tmp/var)
     6: 0010 - 0013 (tmp/var)

plus1:
     ; (lines=3, args=1, vars=1, tmps=%d)
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
