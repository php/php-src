--TEST--
Match expression string jump table
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
zend_test.observer.enabled=0
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
     ; (lines=16, args=0, vars=1, tmps=2)
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
0014 DEFER_RUN
0015 RETURN int(1)
LIVE RANGES:
     1: 0005 - 0013 (loop)

test:
     ; (lines=16, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %s
0000 CV0($char) = RECV 1
0001 MATCH CV0($char) "a": 0003, "b": 0005, "c": 0005, "d": 0007, "e": 0009, "f": 0009, "g": 0011, "h": 0013, "i": 0013, default: 0002
0002 MATCH_ERROR CV0($char)
0003 T1 = QM_ASSIGN string("a")
0004 JMP 0014
0005 T1 = QM_ASSIGN string("b, c")
0006 JMP 0014
0007 T1 = QM_ASSIGN string("d")
0008 JMP 0014
0009 T1 = QM_ASSIGN string("e, f")
0010 JMP 0014
0011 T1 = QM_ASSIGN string("g")
0012 JMP 0014
0013 T1 = QM_ASSIGN string("h, i")
0014 DEFER_RUN
0015 RETURN T1
LIVE RANGES:
     1: 0014 - 0015 (tmp/var)
string(1) "a"
string(4) "b, c"
string(4) "b, c"
string(1) "d"
string(4) "e, f"
string(4) "e, f"
string(1) "g"
string(4) "h, i"
string(4) "h, i"
