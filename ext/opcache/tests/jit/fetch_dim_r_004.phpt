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

Warning: Illegal string offset 'ab' in %sfetch_dim_r_004.php on line 4
string(1) "A"

Warning: Illegal string offset 'ab' in %sfetch_dim_r_004.php on line 4
string(1) "A"

Notice: A non well formed numeric value encountered in %sfetch_dim_r_004.php on line 4
string(1) "C"

Notice: A non well formed numeric value encountered in %sfetch_dim_r_004.php on line 4
string(1) "C"
