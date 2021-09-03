--TEST--
Match expression string jump table
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
        'a' => 'a',
        'b', 'c' => 'b, c',
        'd' => 'd',
        'e', 'f' => 'e, f',
        'g' => 'g',
        'h', 'i' => 'h, i',
    };
}

foreach (range('a', 'i') as $char) {
    var_dump(test($char));
}

?>
--EXPECTF--
$_main:
     ; (lines=15, args=0, vars=1, tmps=2)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 2 %d string("range")
0001 SEND_VAL string("a") 1
0002 SEND_VAL string("i") 2
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
0001 MATCH CV0($char) "a": 0003, "b": 0004, "c": 0004, "d": 0005, "e": 0006, "f": 0006, "g": 0007, "h": 0008, "i": 0008, default: 0002
0002 MATCH_ERROR CV0($char)
0003 RETURN string("a")
0004 RETURN string("b, c")
0005 RETURN string("d")
0006 RETURN string("e, f")
0007 RETURN string("g")
0008 RETURN string("h, i")
string(1) "a"
string(4) "b, c"
string(4) "b, c"
string(1) "d"
string(4) "e, f"
string(4) "e, f"
string(1) "g"
string(4) "h, i"
string(4) "h, i"
