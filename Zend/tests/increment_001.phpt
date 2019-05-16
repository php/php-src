--TEST--
incrementing different variables
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
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
	PHP_INT_MAX,
	(string)PHP_INT_MAX
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
float(2147483648)
float(2147483648)
Done
