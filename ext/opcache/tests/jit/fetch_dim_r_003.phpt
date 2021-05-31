--TEST--
JIT FETCH_DIM_R: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
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
}
foo();
?>
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
Cannot access offset of type string on string
Cannot access offset of type string on string

Warning: Illegal string offset "2x" in %sfetch_dim_r_003.php on line 24
string(1) "C"

Warning: Illegal string offset "2x" in %sfetch_dim_r_003.php on line 27
string(1) "C"
