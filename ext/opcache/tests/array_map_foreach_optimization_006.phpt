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
0003 T3 = DO_ICALL
0004 ASSIGN CV0($array) T3
0005 ASSIGN CV1($plus1) string("plus1")
0006 T4 = QM_ASSIGN CV1($plus1)
0007 TYPE_ASSERT 131079 string("array_map") CV0($array)
0008 T3 = INIT_ARRAY 0 (packed) NEXT
0009 V5 = FE_RESET_R CV0($array) 0017
0010 T7 = FE_FETCH_R V5 T6 0017
0011 T8 = COPY_TMP T4
0012 INIT_DYNAMIC_CALL 1 T8
0013 SEND_VAL_EX T6 1
0014 T6 = DO_FCALL
0015 T3 = ADD_ARRAY_ELEMENT T6 T7
0016 JMP 0010
0017 FE_FREE V5
0018 FREE T4
0019 ASSIGN CV2($foo) T3
0020 INIT_FCALL 1 %d string("var_dump")
0021 SEND_VAR CV2($foo) 1
0022 DO_ICALL
0023 RETURN int(1)
LIVE RANGES:
     4: 0007 - 0018 (tmp/var)
     3: 0009 - 0019 (tmp/var)
     5: 0010 - 0017 (loop)
     6: 0011 - 0013 (tmp/var)
     7: 0011 - 0015 (tmp/var)

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
