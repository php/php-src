--TEST--
array_map(): foreach optimization - PFA
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

$foo = array_map(plusn(?, 2), $array);

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
0003 T2 = DO_ICALL
0004 ASSIGN CV0($array) T2
0005 TYPE_ASSERT 131079 string("array_map") CV0($array)
0006 T2 = INIT_ARRAY 0 (packed) NEXT
0007 V3 = FE_RESET_R CV0($array) 0015
0008 T5 = FE_FETCH_R V3 T4 0015
0009 INIT_FCALL 2 %d string("plusn")
0010 SEND_VAL T4 1
0011 SEND_VAL int(2) 2
0012 T4 = DO_UCALL
0013 T2 = ADD_ARRAY_ELEMENT T4 T5
0014 JMP 0008
0015 FE_FREE V3
0016 ASSIGN CV1($foo) T2
0017 INIT_FCALL 1 %d string("var_dump")
0018 SEND_VAR CV1($foo) 1
0019 DO_ICALL
0020 RETURN int(1)
LIVE RANGES:
     2: 0007 - 0016 (tmp/var)
     3: 0008 - 0015 (loop)
     4: 0009 - 0010 (tmp/var)
     5: 0009 - 0013 (tmp/var)

plusn:
     ; (lines=4, args=2, vars=2, tmps=1)
     ; (after optimizer)
     ; %s
0000 CV0($x) = RECV 1
0001 CV1($n) = RECV 2
0002 T2 = ADD CV0($x) CV1($n)
0003 RETURN T2
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
