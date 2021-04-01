--TEST--
Match expression long jump table
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php

function test($char) {
    return match ($char) {
        1 => '1',
        2, 3 => '2, 3',
        4 => '4',
        5, 6 => '5, 6',
        7 => '7',
        8, 9 => '8, 9',
        default => 'default'
    };
}

foreach (range(0, 10) as $char) {
    var_dump(test($char));
}

?>
--EXPECTF--
$_main:
     ; (lines=15, args=0, vars=1, tmps=2)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 2 %d string("range")
0001 SEND_VAL int(0) 1
0002 SEND_VAL int(10) 2
0003 V2 = DO_ICALL
0004 V1 = FE_RESET_R V2 0013
0005 FE_FETCH_R V1 CV0($char) 0013
0006 INIT_FCALL 1 %d string("var_dump")
0007 INIT_FCALL 1 %d string("test")
0008 SEND_VAR CV0($char) 1
0009 V2 = DO_UCALL
0010 SEND_VAR V2 1
0011 DO_ICALL
0012 JMP 0005
0013 FE_FREE V1
0014 RETURN int(1)
LIVE RANGES:
     1: 0005 - 0013 (loop)

test:
     ; (lines=9, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s
0000 CV0($char) = RECV 1
0001 MATCH CV0($char) 1: 0002, 2: 0003, 3: 0003, 4: 0004, 5: 0005, 6: 0005, 7: 0006, 8: 0007, 9: 0007, default: 0008
0002 RETURN string("1")
0003 RETURN string("2, 3")
0004 RETURN string("4")
0005 RETURN string("5, 6")
0006 RETURN string("7")
0007 RETURN string("8, 9")
0008 RETURN string("default")
string(7) "default"
string(1) "1"
string(4) "2, 3"
string(4) "2, 3"
string(1) "4"
string(4) "5, 6"
string(4) "5, 6"
string(1) "7"
string(4) "8, 9"
string(4) "8, 9"
string(7) "default"
