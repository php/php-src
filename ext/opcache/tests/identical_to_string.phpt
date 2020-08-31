--TEST--
Test optimized opcodes for ==/=== with string literal.
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test(string $a) {
    return $a == 'value';
}
function test1(string $a) {
    return $a == '1';
}
function test2(int|string|false $b) {
    return $b != '0';
}
function test3(int|string|false $b) {
    return $b === '0';
}
function test4(int|string|false $b) {
    return $b !== '0';
}
var_dump(test(''));
var_dump(test('1'));
var_dump(test('value'));
echo "test1\n";
var_dump(test1(''));
var_dump(test1('1'));
var_dump(test1('value'));
echo "test2\n";
var_dump(test2(0));
var_dump(test2('0'));
var_dump(test2(false));
var_dump(test2(1));
var_dump(test2('1'));
var_dump(test2('1'));
var_dump(test2(''));
var_dump(test2(''));
echo "test3\n";
var_dump(test3(0));
var_dump(test3('0'));
var_dump(test3(false));
var_dump(test3(1));
var_dump(test3('1'));
var_dump(test3('1'));
var_dump(test3(''));
var_dump(test3(''));
echo "test4\n";
var_dump(test4(0));
var_dump(test4('0'));
var_dump(test4(false));
var_dump(test4(1));
var_dump(test4('1'));
var_dump(test4('1'));
var_dump(test4(''));
var_dump(test4(''));
--EXPECT--
bool(false)
bool(false)
bool(true)
test1
bool(false)
bool(true)
bool(false)
test2
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
test3
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
test4
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
