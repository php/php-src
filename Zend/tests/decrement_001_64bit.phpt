--TEST--
decrementing different variables
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--INI--
precision=14
--FILE--
<?php

$a = array(
	array(1,2,3),
	"",
	1,
	2.5,
	0,
	"string",
	"123",
	"2.5",
	NULL,
	true,
	false,
	new stdclass,
	array(),
	-PHP_INT_MAX-1,
	(string)(-PHP_INT_MAX-1),
);

foreach ($a as $var) {
	$var--;
	var_dump($var);
}

echo "Done\n";
?>
--EXPECTF--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
int(-1)
int(0)
float(1.5)
int(-1)
string(6) "string"
int(122)
float(1.5)
NULL
bool(true)
bool(false)
object(stdClass)#%d (0) {
}
array(0) {
}
float(-9.2233720368548E+18)
float(-9.2233720368548E+18)
Done
