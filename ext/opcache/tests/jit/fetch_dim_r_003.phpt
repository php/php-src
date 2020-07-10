--TEST--
JIT FETCH_DIM_R: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
;opcache.jit_debug=257
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo() {
    $a = "ABCDEF";
    var_dump($a[0]);
    var_dump($a[2]);
    var_dump($a[1.0]);
    var_dump($a["0"]);
    var_dump($a["2"]);
    var_dump($a[false]);
    var_dump($a[true]);
    var_dump($a[null]);
    try {
        var_dump($a["ab"]);
    } catch (\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    $x = "a";
    $y = "b";
    try {
        var_dump($a[$x . $y]);
    } catch (\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    var_dump($a["2x"]);
    $x = "2";
    $y = "x";
    var_dump($a[$x . $y]);
    try {
        var_dump($a["5.5"]);
    } catch (\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    $x = "5.";
    $y = "5";
    try {
        var_dump($a[$x . $y]);
    } catch (\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        var_dump($a["5.5cx"]);
    } catch (\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        $x = "5.5";
        $y = "c";
        var_dump($a[$x . $y]);
    } catch (\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}
foo();
--EXPECTF--
string(1) "A"
string(1) "C"

Warning: String offset cast occurred in %s on line %d
string(1) "B"
string(1) "A"
string(1) "C"

Warning: String offset cast occurred in %s on line %d
string(1) "A"

Warning: String offset cast occurred in %s on line %d
string(1) "B"

Warning: String offset cast occurred in %s on line %d
string(1) "A"
Illegal offset type
Illegal offset type

Warning: Illegal string offset "2x" in %s on line %d
string(1) "C"

Warning: Illegal string offset "2x" in %s on line %d
string(1) "C"

Warning: String offset cast occurred in %s on line %d
string(1) "F"

Warning: String offset cast occurred in %s on line %d
string(1) "F"
Illegal offset type
Illegal offset type
