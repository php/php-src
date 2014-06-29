--TEST--
incrementing different variables
--INI--
precision=14
--FILE--
<?php

const PHP_INT_MAX32 = 0x7FFFFFFF;
const PHP_INT_MAX64 = 0x7FFFFFFFFFFFFFFF;

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
	PHP_INT_MAX32,
	(string)PHP_INT_MAX32,
	PHP_INT_MAX64,
	(string)PHP_INT_MAX64
);

foreach ($a as $var) {
	$var++;
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
string(1) "1"
int(2)
float(3.5)
int(1)
string(6) "strinh"
int(124)
float(3.5)
int(1)
bool(true)
bool(false)
object(stdClass)#%d (0) {
}
array(0) {
}
int(2147483648)
int(2147483648)
int(9223372036854775808)
int(9223372036854775808)
Done
