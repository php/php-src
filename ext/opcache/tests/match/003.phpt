--TEST--
Match expression long jump table
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
     ; (lines=16, args=0, vars=1, tmps=2)
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
0014 DEFER_RUN
0015 RETURN int(1)
LIVE RANGES:
     1: 0005 - 0013 (loop)

test:
     ; (lines=17, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %s
0000 CV0($char) = RECV 1
0001 MATCH CV0($char) 1: 0002, 2: 0004, 3: 0004, 4: 0006, 5: 0008, 6: 0008, 7: 0010, 8: 0012, 9: 0012, default: 0014
0002 T1 = QM_ASSIGN string("1")
0003 JMP 0015
0004 T1 = QM_ASSIGN string("2, 3")
0005 JMP 0015
0006 T1 = QM_ASSIGN string("4")
0007 JMP 0015
0008 T1 = QM_ASSIGN string("5, 6")
0009 JMP 0015
0010 T1 = QM_ASSIGN string("7")
0011 JMP 0015
0012 T1 = QM_ASSIGN string("8, 9")
0013 JMP 0015
0014 T1 = QM_ASSIGN string("default")
0015 DEFER_RUN
0016 RETURN T1
LIVE RANGES:
     1: 0015 - 0016 (tmp/var)
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
