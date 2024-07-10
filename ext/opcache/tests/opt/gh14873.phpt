--TEST--
GH-14873 (PHP 8.4 min function fails on typed integer)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x7FFEBFFF
opcache.opt_debug_level=0x20000
--FILE--
<?php

function testTrim1(string $value): string {
	$value = trim($value);
	return $value;
}

function testMin2(int $value): int {
	$value = min($value, 100);
	return $value;
}

function testMin2_TMP(int $value): int {
	$value = min($value + 1, 100);
	return $value;
}

function testStrstr3(string $value): string {
	$value = strstr($value, "needle", false);
	return $value;
}

var_dump(testTrim1(" boo "));
var_dump(testMin2(5));
var_dump(testMin2_TMP(5));
var_dump(testStrstr3("needles"));

?>
--EXPECTF--
$_main:
     ; (lines=25, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 1 %d string("var_dump")
0001 INIT_FCALL 1 %d string("testtrim1")
0002 SEND_VAL string(" boo ") 1
0003 V0 = DO_UCALL
0004 SEND_VAR V0 1
0005 DO_ICALL
0006 INIT_FCALL 1 %d string("var_dump")
0007 INIT_FCALL 1 %d string("testmin2")
0008 SEND_VAL int(5) 1
0009 V0 = DO_UCALL
0010 SEND_VAR V0 1
0011 DO_ICALL
0012 INIT_FCALL 1 %d string("var_dump")
0013 INIT_FCALL 1 %d string("testmin2_tmp")
0014 SEND_VAL int(5) 1
0015 V0 = DO_UCALL
0016 SEND_VAR V0 1
0017 DO_ICALL
0018 INIT_FCALL 1 %d string("var_dump")
0019 INIT_FCALL 1 %d string("teststrstr3")
0020 SEND_VAL string("needles") 1
0021 V0 = DO_UCALL
0022 SEND_VAR V0 1
0023 DO_ICALL
0024 RETURN int(1)

testTrim1:
     ; (lines=4, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s
0000 CV0($value) = RECV 1
0001 T1 = FRAMELESS_ICALL_1(trim) CV0($value)
0002 ASSIGN CV0($value) T1
0003 RETURN CV0($value)

testMin2:
     ; (lines=5, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s
0000 CV0($value) = RECV 1
0001 T1 = FRAMELESS_ICALL_2(min) CV0($value) int(100)
0002 CV0($value) = QM_ASSIGN T1
0003 VERIFY_RETURN_TYPE CV0($value)
0004 RETURN CV0($value)

testMin2_TMP:
     ; (lines=5, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s
0000 CV0($value) = RECV 1
0001 T1 = ADD CV0($value) int(1)
0002 CV0($value) = FRAMELESS_ICALL_2(min) T1 int(100)
0003 VERIFY_RETURN_TYPE CV0($value)
0004 RETURN CV0($value)

testStrstr3:
     ; (lines=6, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %s
0000 CV0($value) = RECV 1
0001 T1 = FRAMELESS_ICALL_3(strstr) CV0($value) string("needle")
0002 OP_DATA bool(false)
0003 ASSIGN CV0($value) T1
0004 VERIFY_RETURN_TYPE CV0($value)
0005 RETURN CV0($value)
LIVE RANGES:
     1: 0002 - 0003 (tmp/var)
string(3) "boo"
int(5)
int(6)
string(7) "needles"
