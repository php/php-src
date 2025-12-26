--TEST--
Match expression mixed jump table
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
zend_test.observer.enabled=0
--EXTENSIONS--
opcache
--FILE--
<?php

function test($value) {
    return match ($value) {
        1 => '1 int',
        '1' => '1 string',
        2 => '2 int',
        '2' => '2 string',
        3 => '3 int',
        '3' => '3 string',
        4 => '4 int',
        '4' => '4 string',
        5 => '5 int',
        '5' => '5 string',
        default => 'default',
    };
}

foreach (range(0, 6) as $number) {
    var_dump(test($number));
    var_dump(test((string) $number));
}

?>
--EXPECTF--
$_main:
     ; (lines=23, args=0, vars=1, tmps=2)
     ; (after optimizer)
     ; %s.php:1-25
0000 INIT_FCALL 2 %d string("range")
0001 SEND_VAL int(0) 1
0002 SEND_VAL int(6) 2
0003 V2 = DO_ICALL
0004 V1 = FE_RESET_R V2 0020
0005 FE_FETCH_R V1 CV0($number) 0020
0006 INIT_FCALL 1 %d string("var_dump")
0007 INIT_FCALL 1 %d string("test")
0008 SEND_VAR CV0($number) 1
0009 V2 = DO_UCALL
0010 SEND_VAR V2 1
0011 DO_ICALL
0012 INIT_FCALL 1 %d string("var_dump")
0013 INIT_FCALL 1 %d string("test")
0014 T2 = CAST (string) CV0($number)
0015 SEND_VAL T2 1
0016 V2 = DO_UCALL
0017 SEND_VAR V2 1
0018 DO_ICALL
0019 JMP 0005
0020 FE_FREE V1
0021 DEFER_RUN
0022 RETURN int(1)
LIVE RANGES:
     1: 0005 - 0020 (loop)

test:
     ; (lines=25, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %s.php:3-17
0000 CV0($value) = RECV 1
0001 MATCH CV0($value) 1: 0002, "1": 0004, 2: 0006, "2": 0008, 3: 0010, "3": 0012, 4: 0014, "4": 0016, 5: 0018, "5": 0020, default: 0022
0002 T1 = QM_ASSIGN string("1 int")
0003 JMP 0023
0004 T1 = QM_ASSIGN string("1 string")
0005 JMP 0023
0006 T1 = QM_ASSIGN string("2 int")
0007 JMP 0023
0008 T1 = QM_ASSIGN string("2 string")
0009 JMP 0023
0010 T1 = QM_ASSIGN string("3 int")
0011 JMP 0023
0012 T1 = QM_ASSIGN string("3 string")
0013 JMP 0023
0014 T1 = QM_ASSIGN string("4 int")
0015 JMP 0023
0016 T1 = QM_ASSIGN string("4 string")
0017 JMP 0023
0018 T1 = QM_ASSIGN string("5 int")
0019 JMP 0023
0020 T1 = QM_ASSIGN string("5 string")
0021 JMP 0023
0022 T1 = QM_ASSIGN string("default")
0023 DEFER_RUN
0024 RETURN T1
LIVE RANGES:
     1: 0023 - 0024 (tmp/var)
string(7) "default"
string(7) "default"
string(5) "1 int"
string(8) "1 string"
string(5) "2 int"
string(8) "2 string"
string(5) "3 int"
string(8) "3 string"
string(5) "4 int"
string(8) "4 string"
string(5) "5 int"
string(8) "5 string"
string(7) "default"
string(7) "default"
