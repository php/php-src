--TEST--
array_map(): foreach optimization - unused refcounted result
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

function stdClass() {
    return new stdClass();
}

$array = range(1, 10);

array_map(stdClass(...), $array);

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 2 %d string("range")
0001 SEND_VAL int(1) 1
0002 SEND_VAL int(10) 2
0003 T1 = DO_ICALL
0004 ASSIGN CV0($array) T1
0005 TYPE_ASSERT 131079 string("array_map") CV0($array)
0006 T1 = INIT_ARRAY 0 (packed) NEXT
0007 V2 = FE_RESET_R CV0($array) 0014
0008 T4 = FE_FETCH_R V2 T3 0014
0009 INIT_FCALL 1 %d string("stdclass")
0010 SEND_VAL T3 1
0011 T3 = DO_UCALL
0012 T1 = ADD_ARRAY_ELEMENT T3 T4
0013 JMP 0008
0014 FE_FREE V2
0015 FREE T1
0016 RETURN int(1)
LIVE RANGES:
     1: 0007 - 0015 (tmp/var)
     2: 0008 - 0014 (loop)
     3: 0009 - 0010 (tmp/var)
     4: 0009 - 0012 (tmp/var)

stdClass:
     ; (lines=3, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s
0000 T0 = NEW 0 string("stdClass")
0001 DO_FCALL
0002 RETURN T0
LIVE RANGES:
     0: 0001 - 0002 (new)
