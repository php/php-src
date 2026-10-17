--TEST--
JIT: BW_NOT (~) on integers in a hot loop (function JIT), with type changes
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=32M
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
--FILE--
<?php

function not_cv(int $x): int {
    $y = ~$x;          // result in a CV
    return $y;
}

function not_tmp(int $x): int {
    return ~$x & 0xFF; // result in a TMP
}

function not_twice(int $x): int {
    return ~~$x;
}

function not_chain(int $a, int $b): int {
    return ~$a & $b;   // and-not combination
}

$values = [0, 1, -1, 2, 255, PHP_INT_MAX, PHP_INT_MIN, PHP_INT_MAX - 1, PHP_INT_MIN + 1,
           0x123456789ABCDEF, -0x123456789ABCDEF];
mt_srand(7);
for ($i = 0; $i < 100; $i++) {
    $values[] = (mt_rand() << 33) ^ (mt_rand() << 11) ^ mt_rand();
}

$bad = 0;
for ($round = 0; $round < 20; $round++) {
    foreach ($values as $v) {
        // ~x == -x - 1 for every int except PHP_INT_MIN (where -x overflows)
        $expected = ($v === PHP_INT_MIN) ? PHP_INT_MAX : -$v - 1;
        if (not_cv($v) !== $expected) $bad++;
        if (not_tmp($v) !== ($expected & 0xFF)) $bad++;
        if (not_twice($v) !== $v) $bad++;
        if (not_chain($v, 0x0F0F) !== ($expected & 0x0F0F)) $bad++;
    }
}
var_dump($bad);

// Same loop bodies, now fed other types after the trace was compiled for int.
function not_any($x) {
    return ~$x;
}
for ($i = 0; $i < 200; $i++) {
    not_any($i);
}
var_dump(not_any(5));
var_dump(not_any(2.0));
var_dump(not_any("ab") === "\x9E\x9D");
foreach ([null, true, false, [], new stdClass] as $bad_value) {
    try {
        not_any($bad_value);
        echo "no exception\n";
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
var_dump(not_any(7));

// Undefined variable operand
function not_undef() {
    return ~$undefined;
}
for ($i = 0; $i < 3; $i++) {
    try {
        not_undef();
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
int(0)
int(-6)
int(-3)
bool(true)
Cannot perform bitwise not on null
Cannot perform bitwise not on true
Cannot perform bitwise not on false
Cannot perform bitwise not on array
Cannot perform bitwise not on stdClass
int(-8)

Warning: Undefined variable $undefined in %s on line %d
Cannot perform bitwise not on null

Warning: Undefined variable $undefined in %s on line %d
Cannot perform bitwise not on null

Warning: Undefined variable $undefined in %s on line %d
Cannot perform bitwise not on null
