--TEST--
array_map(): foreach optimization - PFAs with literal args are optimizable
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--ENV--
A=1
--FILE--
<?php

if (getenv('A')) {
    function plusn($x, $n) {
        return $x + $n;
    }
}

var_dump(array_map(plusn(?, 2), range(1, 2)));

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 1 %d string("getenv")
0001 SEND_VAL string("A") 1
0002 T0 = DO_ICALL
0003 JMPZ T0 0005
0004 DECLARE_FUNCTION string("plusn") 0
0005 INIT_FCALL 1 %d string("var_dump")
0006 INIT_FCALL 2 %d string("range")
0007 SEND_VAL int(1) 1
0008 SEND_VAL int(2) 2
0009 T3 = DO_ICALL
0010 TYPE_ASSERT 131079 string("array_map") T3
0011 T0 = INIT_ARRAY 0 (packed) NEXT
0012 V1 = FE_RESET_R T3 0020
0013 T3 = FE_FETCH_R V1 T2 0020
0014 INIT_FCALL_BY_NAME 2 string("plusn")
0015 SEND_VAL_EX T2 1
0016 SEND_VAL_EX int(2) 2
0017 T2 = DO_FCALL_BY_NAME
0018 T0 = ADD_ARRAY_ELEMENT T2 T3
0019 JMP 0013
0020 FE_FREE V1
0021 SEND_VAL T0 1
0022 DO_ICALL
0023 RETURN int(1)
LIVE RANGES:
     3: 0010 - 0012 (tmp/var)
     0: 0012 - 0021 (tmp/var)
     1: 0013 - 0020 (loop)

plusn:
     ; (lines=4, args=2, vars=2, tmps=%d)
     ; (after optimizer)
     ; %s
0000 CV0($x) = RECV 1
0001 CV1($n) = RECV 2
0002 T2 = ADD CV0($x) CV1($n)
0003 RETURN T2
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(4)
}
