--TEST--
Bug #bug69017 - Fail to push to the empty array with the constant value defined in class scope
--FILE--
<?php

class c1
{
	const ZERO = 0;
	const ONE = 1;
	public static $a1 = array(self::ONE => 'one');
	public static $a2 = array(self::ZERO => 'zero');
}

$v1 = c1::$a1;
$v2 = c1::$a2;

$v1[] = 1;
$v2[] = 1;

var_dump($v1);
var_dump($v2);
?>
--EXPECT--
array(2) {
  [1]=>
  string(3) "one"
  [2]=>
  int(1)
}
array(2) {
  [0]=>
  string(4) "zero"
  [1]=>
  int(1)
}
