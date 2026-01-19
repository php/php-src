--TEST--
array_map(): foreach optimization - Error
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

$array = \range(1, 10);

$foo = array_map(
     plus1(...),
     new \ArrayIterator($array)
);

\var_dump($foo);

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
0005 V5 = NEW 1 string("ArrayIterator")
0006 SEND_VAR%S CV0($array) 1
0007 DO_FCALL
0008 TYPE_ASSERT 131079 string("array_map") V5
0009 T2 = INIT_ARRAY 0 (packed) NEXT
0010 V3 = FE_RESET_R V5 0017
0011 T5 = FE_FETCH_R V3 T4 0017
0012 INIT_FCALL 1 %d string("plus1")
0013 SEND_VAL T4 1
0014 V4 = DO_UCALL
0015 T2 = ADD_ARRAY_ELEMENT V4 T5
0016 JMP 0011
0017 FE_FREE V3
0018 ASSIGN CV1($foo) T2
0019 INIT_FCALL 1 %d string("var_dump")
0020 SEND_VAR CV1($foo) 1
0021 DO_ICALL
0022 RETURN int(1)
LIVE RANGES:
     5: 0006 - 0008 (new)
     5: 0008 - 0010 (tmp/var)
     2: 0010 - 0018 (tmp/var)
     3: 0011 - 0017 (loop)
     4: 0012 - 0013 (tmp/var)
     5: 0012 - 0015 (tmp/var)

plus1:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %s
0000 CV0($x) = RECV 1
0001 T1 = ADD CV0($x) int(1)
0002 RETURN T1

Fatal error: Uncaught TypeError: array_map(): Argument #2 ($array) must be of type array, ArrayIterator given in %s:9
Stack trace:
#0 {main}
  thrown in %s on line 9
