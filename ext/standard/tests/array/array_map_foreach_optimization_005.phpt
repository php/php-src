--TEST--
array_map(): foreach optimization - static call
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

class Adder {
    static function plus1($x) {
        return $x + 1;
    }
}

$array = range(1, 10);

$foo = array_map(Adder::plus1(...), $array);

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
0005 TYPE_ASSERT 131079 string("array_map") CV0($array)
0006 T2 = INIT_ARRAY 0 (packed) NEXT
0007 V3 = FE_RESET_R CV0($array) 0014
0008 T5 = FE_FETCH_R V3 T4 0014
0009 INIT_STATIC_METHOD_CALL 1 string("Adder") string("plus1")
0010 SEND_VAL T4 1
0011 V4 = DO_UCALL
0012 T2 = ADD_ARRAY_ELEMENT V4 T5
0013 JMP 0008
0014 FE_FREE V3
0015 ASSIGN CV1($foo) T2
0016 INIT_FCALL 1 %d string("var_dump")
0017 SEND_VAR CV1($foo) 1
0018 DO_ICALL
0019 RETURN int(1)
LIVE RANGES:
     2: 0007 - 0015 (tmp/var)
     3: 0008 - 0014 (loop)
     4: 0009 - 0010 (tmp/var)
     5: 0009 - 0012 (tmp/var)

Adder::plus1:
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
