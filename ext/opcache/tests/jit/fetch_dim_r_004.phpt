--TEST--
JIT FETCH_DIM_R: 004
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
function foo($n) {
    $a = "ABCDEF";
    try {
        var_dump($a[$n]);
    } catch (\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}
foo(0);
foo(2);
foo(1.0);
foo("0");
foo("2");
foo(false);
foo(true);
foo(null);
foo("ab");
$x="a";
$y="b";
foo($x.$y);
foo("2x");
$x=2;
$y="x";
foo($x.$y);
foo("5.5");
$x = "5.";
$y = "5";
foo($x.$y);
try {
    foo("5.5c");
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $x = "5.5";
    $y = "c";
    foo($x.$y);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
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
