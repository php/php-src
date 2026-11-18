--TEST--
$i++ < $n must not be optimized to ++$i <= $n unless both operands are integers that cannot overflow
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php

function half(array $a) {
    $n = count($a) / 2;
    $i = 0;
    $o = [];
    while ($i++ < $n) {
        $o[] = $a[$i - 1];
    }
    return $o;
}

function float_bound(float $n) {
    $c = 0;
    for ($i = 0; $i++ < $n;) $c++;
    return $c;
}

function numeric_string_bound(string $n) {
    $c = 0;
    for ($i = 0; $i++ < $n;) $c++;
    return $c;
}

function float_counter() {
    $c = 0;
    $x = 0.5;
    while ($x++ < 3) $c++;
    return $c;
}

function null_bound(int $i, ?int $n) {
    return $i++ < $n;
}

function bool_bound(int $i, bool $n) {
    return $i++ < $n;
}

function post_dec_float_bound(float $min) {
    $c = 0;
    $i = 3;
    while ($min < $i--) $c++;
    return $c;
}

function inc_overflow(int $i, int $n) {
    $c = 0;
    while ($i++ < $n && $c < 5) $c++;
    return $c;
}

function dec_overflow(int $i, int $n) {
    $c = 0;
    while ($n < $i-- && $c < 5) $c++;
    return $c;
}

function int_bound(int $n) {
    $c = 0;
    for ($i = 0; $i++ < $n;) $c++;
    return $c;
}

function literal_bound() {
    $c = 0;
    for ($i = 0; $i++ < 3;) $c++;
    return $c;
}

function post_dec_int_bound(int $min) {
    $c = 0;
    $i = 3;
    while ($min < $i--) $c++;
    return $c;
}

var_dump(half([1, 2, 3, 4, 5]));
var_dump(float_bound(2.5));
var_dump(numeric_string_bound("2.5"));
var_dump(float_counter());
var_dump(null_bound(-1, null));
var_dump(bool_bound(-1, false));
var_dump(post_dec_float_bound(0.5));
var_dump(inc_overflow(PHP_INT_MAX - 1, PHP_INT_MAX));
var_dump(dec_overflow(PHP_INT_MIN + 1, PHP_INT_MIN));
var_dump(int_bound(3));
var_dump(literal_bound());
var_dump(post_dec_int_bound(0));

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
int(3)
int(3)
int(3)
bool(false)
bool(false)
int(3)
int(1)
int(1)
int(3)
int(3)
int(3)
