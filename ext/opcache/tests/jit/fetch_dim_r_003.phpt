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

Warning: Illegal string offset 'ab' in %sfetch_dim_r_003.php on line 12
string(1) "A"

Warning: Illegal string offset 'ab' in %sfetch_dim_r_003.php on line 15
string(1) "A"

Notice: A non well formed numeric value encountered in %sfetch_dim_r_003.php on line 16
string(1) "C"

Notice: A non well formed numeric value encountered in %sfetch_dim_r_003.php on line 19
string(1) "C"
