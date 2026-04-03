--TEST--
Return type check converted to a cast
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php

function strToBool(string $v): bool {
	return $v;
}
function intToBool(int $v): bool {
	return $v;
}
function floatToBool(float $v): bool {
	return $v;
}

function strToInt(string $v): int {
	return $v;
}
function floatToInt(float $v): int {
	return $v;
}
function boolToInt(bool $v): int {
	return $v;
}

function strToFloat(string $v): float {
	return $v;
}
function intToFloat(int $v): float {
	return $v;
}
function boolToFloat(bool $v): float {
	return $v;
}

function floatToString(float $v): string {
	return $v;
}
function intToString(int $v): string {
	return $v;
}
function boolToString(bool $v): string {
	return $v;
}

function floatToUnion(float $v): string|int|bool {
	return $v;
}
function intToUnion(int $v): string|float|bool {
	return $v;
}
function boolToUnion(bool $v): string|int|float {
	return $v;
}
function stringToUnion(string $v): bool|int|float {
	return $v;
}


?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s:1-58
0000 RETURN int(1)

strToBool:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:3-5
0000 CV0($v) = RECV 1
0001 VERIFY_RETURN_TYPE CV0($v)
0002 RETURN CV0($v)

intToBool:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:6-8
0000 CV0($v) = RECV 1
0001 VERIFY_RETURN_TYPE CV0($v)
0002 RETURN CV0($v)

floatToBool:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:9-11
0000 CV0($v) = RECV 1
0001 VERIFY_RETURN_TYPE CV0($v)
0002 RETURN CV0($v)

strToInt:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:13-15
0000 CV0($v) = RECV 1
0001 VERIFY_RETURN_TYPE CV0($v)
0002 RETURN CV0($v)

floatToInt:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:16-18
0000 CV0($v) = RECV 1
0001 VERIFY_RETURN_TYPE CV0($v)
0002 RETURN CV0($v)

boolToInt:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:19-21
0000 CV0($v) = RECV 1
0001 CAST (long) CV0($v)
0002 RETURN CV0($v)

strToFloat:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:23-25
0000 CV0($v) = RECV 1
0001 VERIFY_RETURN_TYPE CV0($v)
0002 RETURN CV0($v)

intToFloat:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:26-28
0000 CV0($v) = RECV 1
0001 CAST (double) CV0($v)
0002 RETURN CV0($v)

boolToFloat:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:29-31
0000 CV0($v) = RECV 1
0001 CAST (double) CV0($v)
0002 RETURN CV0($v)

floatToString:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:33-35
0000 CV0($v) = RECV 1
0001 VERIFY_RETURN_TYPE CV0($v)
0002 RETURN CV0($v)

intToString:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:36-38
0000 CV0($v) = RECV 1
0001 CAST (string) CV0($v)
0002 RETURN CV0($v)

boolToString:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:39-41
0000 CV0($v) = RECV 1
0001 CAST (string) CV0($v)
0002 RETURN CV0($v)

floatToUnion:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:43-45
0000 CV0($v) = RECV 1
0001 VERIFY_RETURN_TYPE CV0($v)
0002 RETURN CV0($v)

intToUnion:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:46-48
0000 CV0($v) = RECV 1
0001 CAST (double) CV0($v)
0002 RETURN CV0($v)

boolToUnion:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:49-51
0000 CV0($v) = RECV 1
0001 CAST (long) CV0($v)
0002 RETURN CV0($v)

stringToUnion:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s:52-54
0000 CV0($v) = RECV 1
0001 VERIFY_RETURN_TYPE CV0($v)
0002 RETURN CV0($v)
