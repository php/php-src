--TEST--
JIT FETCH_DIM_R: 002
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
    $a = array(1,2,3,""=>4,"ab"=>5,"2x"=>6);
    var_dump($a[$n]);
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
