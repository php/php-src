--TEST--
JIT ASSIGN_DIM: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--SKIPIF--
<?php require_once('../skipif.inc'); ?>
--FILE--
<?php
function foo() {
	$var[] = 1;
	var_dump($var);
}
foo();

function foo1() {
	$var1[] = true;
	var_dump($var1);
}
foo1();

function foo2() {
	$var2[] = array();
	var_dump($var2);
}
foo2();

function foo3() {
	$array = array(PHP_INT_MAX => "dummy");
	$array[] = array();

	$array = new ArrayObject();
	$array[index()] = 1;
	$array[offset()] = 2;

	var_dump($array);
}
foo3();

function index() {
	return 2;
}

function offset() {
	return "a";
}

function foo4() {
	$array = array();
	$array[] = array();
	$array[0][] = 1;
	$array[0][1] = 1;
	var_dump($array);

	$array[function() {}] = 2;
	var_dump($array);

	$array2[][] = 3;
	var_dump($array);
}
foo4();

function foo5() {
   $a = 1;
   $a[2] = 1;
   return $a;
}
var_dump(foo5());

--EXPECTF--
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  bool(true)
}
array(1) {
  [0]=>
  array(0) {
  }
}

Warning: Cannot add element to the array as the next element is already occupied in %sassign_dim_002.php on line 22
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(2) {
    [2]=>
    int(1)
    ["a"]=>
    int(2)
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
}

Warning: Illegal offset type in %sassign_dim_002.php on line 47
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
}

Warning: Cannot use a scalar value as an array in %sassign_dim_002.php on line 57
int(1)
