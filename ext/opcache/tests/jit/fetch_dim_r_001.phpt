--TEST--
JIT FETCH_DIM_R: 001
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
    $a = array(1,2,3,""=>4,"ab"=>5,"2x"=>6);
    var_dump($a[0]);
    var_dump($a[2]);
    var_dump($a[1.0]);
    var_dump($a["0"]);
    var_dump($a["2"]);
    var_dump($a[false]);
    var_dump($a[true]);
    var_dump($a[null]);
    var_dump($a["ab"]);
    $x = "a";
    $y = "b";
    var_dump($a[$x . $y]);
    var_dump($a["2x"]);
    $x = "2";
    $y = "x";
    var_dump($a[$x . $y]);
}
foo();
?>
--EXPECT--
int(1)
int(3)
int(2)
int(1)
int(3)
int(1)
int(2)
int(4)
int(5)
int(5)
int(6)
int(6)
