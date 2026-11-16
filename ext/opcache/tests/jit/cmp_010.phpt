--TEST--
JIT CMP: 010 Integer range must not be narrowed by a comparison with a non-integer operand
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_hot_func=1
opcache.jit_hot_loop=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
function lt_float(int $x) {
    if ($x < 1.0e19) {
        return $x + 1;
    }
    return 0;
}
function lt_string(int $x) {
    if ($x < "1e19") {
        return $x + 1;
    }
    return 0;
}
function le_float(int $x) {
    if ($x <= -1.0e19) {
        return 0;
    }
    return $x - 1;
}
foreach ([1, 2, 3, PHP_INT_MAX] as $v) {
    $r = lt_float($v);
}
echo "int < float: ";
var_dump($r);
foreach ([1, 2, 3, PHP_INT_MAX] as $v) {
    $r = lt_string($v);
}
echo "int < numeric string: ";
var_dump($r);
foreach ([1, 2, 3, PHP_INT_MIN] as $v) {
    $r = le_float($v);
}
echo "int <= float (false): ";
var_dump($r);
?>
--EXPECT--
int < float: float(9.223372036854776E+18)
int < numeric string: float(9.223372036854776E+18)
int <= float (false): float(-9.223372036854776E+18)
