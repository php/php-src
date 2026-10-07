--TEST--
SCCP: isset/empty with string and a dimension
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php

function isset_1() {
    $cv = -1;
    return isset("abc"[$cv]);
}

function isset_2() {
    $cv = -2;
    return isset("abc"[$cv]);
}

function isset_3() {
    $cv = -3;
    return isset("abc"[$cv]);
}

function isset_4() {
    $cv = -4;
    return isset("abc"[$cv]);
}

function isset0() {
    $cv = 0;
    return isset("abc"[$cv]);
}

function isset1() {
    $cv = 1;
    return isset("abc"[$cv]);
}

function isset2() {
    $cv = 2;
    return isset("abc"[$cv]);
}

function isset3() {
    $cv = 3;
    return isset("abc"[$cv]);
}

function empty_1() {
    $cv = -1;
    return empty("abc"[$cv]);
}

function empty_2() {
    $cv = -2;
    return empty("abc"[$cv]);
}

function empty_3() {
    $cv = -3;
    return empty("abc"[$cv]);
}

function empty_4() {
    $cv = -4;
    return empty("abc"[$cv]);
}

function empty0() {
    $cv = 0;
    return empty("abc"[$cv]);
}

function empty1() {
    $cv = 1;
    return empty("abc"[$cv]);
}

function empty2() {
    $cv = 2;
    return empty("abc"[$cv]);
}

function empty3() {
    $cv = 3;
    return empty("abc"[$cv]);
}

function empty_special_case() {
    $cv = 1;
    return empty("a0c"[$cv]);
}

var_dump(isset0());
var_dump(isset1());
var_dump(isset2());
var_dump(isset3());
var_dump(isset_1());
var_dump(isset_2());
var_dump(isset_3());
var_dump(isset_4());

var_dump(empty0());
var_dump(empty1());
var_dump(empty2());
var_dump(empty3());
var_dump(empty_1());
var_dump(empty_2());
var_dump(empty_3());
var_dump(empty_4());
var_dump(empty_special_case());

?>
--EXPECTF--
$_main:
     ; (lines=86, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 INIT_FCALL 1 %d string("var_dump")
0001 INIT_FCALL 0 %d string("isset0")
0002 T0 = DO_UCALL
0003 SEND_VAL T0 1
0004 DO_ICALL
0005 INIT_FCALL 1 %d string("var_dump")
0006 INIT_FCALL 0 %d string("isset1")
0007 T0 = DO_UCALL
0008 SEND_VAL T0 1
0009 DO_ICALL
0010 INIT_FCALL 1 %d string("var_dump")
0011 INIT_FCALL 0 %d string("isset2")
0012 T0 = DO_UCALL
0013 SEND_VAL T0 1
0014 DO_ICALL
0015 INIT_FCALL 1 %d string("var_dump")
0016 INIT_FCALL 0 %d string("isset3")
0017 T0 = DO_UCALL
0018 SEND_VAL T0 1
0019 DO_ICALL
0020 INIT_FCALL 1 %d string("var_dump")
0021 INIT_FCALL 0 %d string("isset_1")
0022 T0 = DO_UCALL
0023 SEND_VAL T0 1
0024 DO_ICALL
0025 INIT_FCALL 1 %d string("var_dump")
0026 INIT_FCALL 0 %d string("isset_2")
0027 T0 = DO_UCALL
0028 SEND_VAL T0 1
0029 DO_ICALL
0030 INIT_FCALL 1 %d string("var_dump")
0031 INIT_FCALL 0 %d string("isset_3")
0032 T0 = DO_UCALL
0033 SEND_VAL T0 1
0034 DO_ICALL
0035 INIT_FCALL 1 %d string("var_dump")
0036 INIT_FCALL 0 %d string("isset_4")
0037 T0 = DO_UCALL
0038 SEND_VAL T0 1
0039 DO_ICALL
0040 INIT_FCALL 1 %d string("var_dump")
0041 INIT_FCALL 0 %d string("empty0")
0042 T0 = DO_UCALL
0043 SEND_VAL T0 1
0044 DO_ICALL
0045 INIT_FCALL 1 %d string("var_dump")
0046 INIT_FCALL 0 %d string("empty1")
0047 T0 = DO_UCALL
0048 SEND_VAL T0 1
0049 DO_ICALL
0050 INIT_FCALL 1 %d string("var_dump")
0051 INIT_FCALL 0 %d string("empty2")
0052 T0 = DO_UCALL
0053 SEND_VAL T0 1
0054 DO_ICALL
0055 INIT_FCALL 1 %d string("var_dump")
0056 INIT_FCALL 0 %d string("empty3")
0057 T0 = DO_UCALL
0058 SEND_VAL T0 1
0059 DO_ICALL
0060 INIT_FCALL 1 %d string("var_dump")
0061 INIT_FCALL 0 %d string("empty_1")
0062 T0 = DO_UCALL
0063 SEND_VAL T0 1
0064 DO_ICALL
0065 INIT_FCALL 1 %d string("var_dump")
0066 INIT_FCALL 0 %d string("empty_2")
0067 T0 = DO_UCALL
0068 SEND_VAL T0 1
0069 DO_ICALL
0070 INIT_FCALL 1 %d string("var_dump")
0071 INIT_FCALL 0 %d string("empty_3")
0072 T0 = DO_UCALL
0073 SEND_VAL T0 1
0074 DO_ICALL
0075 INIT_FCALL 1 %d string("var_dump")
0076 INIT_FCALL 0 %d string("empty_4")
0077 T0 = DO_UCALL
0078 SEND_VAL T0 1
0079 DO_ICALL
0080 INIT_FCALL 1 %d string("var_dump")
0081 INIT_FCALL 0 %d string("empty_special_case")
0082 T0 = DO_UCALL
0083 SEND_VAL T0 1
0084 DO_ICALL
0085 RETURN int(1)

isset_1:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(true)

isset_2:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(true)

isset_3:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(true)

isset_4:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(false)

isset0:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(true)

isset1:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(true)

isset2:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(true)

isset3:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(false)

empty_1:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(false)

empty_2:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(false)

empty_3:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(false)

empty_4:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(true)

empty0:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(false)

empty1:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(false)

empty2:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(false)

empty3:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(true)

empty_special_case:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s.php:%s
0000 RETURN bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
