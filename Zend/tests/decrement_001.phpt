--TEST--
decrementing different variables
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
	-0x7FFFFFFF-1,
	(string)(-0x7FFFFFFF-1),
	-0x7FFFFFFFFFFFFFFF-1,
	(string)(-0x7FFFFFFFFFFFFFFF-1),
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
int(-2147483649)
int(-2147483649)
int(-9223372036854775809)
int(-9223372036854775809)
Done
