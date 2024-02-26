--TEST--
Match expression mixed jump table
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
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
     ; (lines=22, args=0, vars=1, tmps=2)
     ; (after optimizer)
     ; %s.php:1-25
0000 INIT_FCALL %d %d %s
0001 SEND_VAL int(0) 1
0002 SEND_VAL int(6) 2
0003 V2 = DO_ICALL
0004 V1 = FE_RESET_R V2 0020
0005 FE_FETCH_R V1 CV0($number) 0020
0006 INIT_FCALL %d %d %s
0007 INIT_FCALL 1 %d string("test")
0008 SEND_VAR CV0($number) 1
0009 V2 = DO_UCALL
0010 SEND_VAR V2 1
0011 DO_ICALL
0012 INIT_FCALL %d %d %s
0013 INIT_FCALL 1 %d string("test")
0014 T2 = CAST (string) CV0($number)
0015 SEND_VAL T2 1
0016 V2 = DO_UCALL
0017 SEND_VAR V2 1
0018 DO_ICALL
0019 JMP 0005
0020 FE_FREE V1
0021 RETURN int(1)
LIVE RANGES:
     1: 0005 - 0020 (loop)

test:
     ; (lines=13, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s.php:3-17
0000 CV0($value) = RECV 1
0001 MATCH CV0($value) 1: 0002, "1": 0003, 2: 0004, "2": 0005, 3: 0006, "3": 0007, 4: 0008, "4": 0009, 5: 0010, "5": 0011, default: 0012
0002 RETURN string("1 int")
0003 RETURN string("1 string")
0004 RETURN string("2 int")
0005 RETURN string("2 string")
0006 RETURN string("3 int")
0007 RETURN string("3 string")
0008 RETURN string("4 int")
0009 RETURN string("4 string")
0010 RETURN string("5 int")
0011 RETURN string("5 string")
0012 RETURN string("default")
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
