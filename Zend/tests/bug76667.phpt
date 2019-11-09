--TEST--
Bug #76667 (Segfault with divide-assign op and __get + __set)
--FILE--
<?php

class T {
	public function __get($k)
	{
		return $undefined->$k;
	}

	public function __set($k, $v)
	{
		return $this->$v /= 0;
	}
};

$x = new T;
$x->x = 1;
?>
--EXPECTF--
Warning: Undefined variable: undefined in %s on line %d

Warning: Trying to get property '1' of non-object in %s on line %d

Warning: Division by zero in %sbug76667.php on line %d

Warning: Undefined variable: undefined in %s on line %d

Warning: Trying to get property 'NAN' of non-object in %s on line %d

Warning: Division by zero in %sbug76667.php on line %d

Warning: Undefined variable: undefined in %s on line %d

Warning: Trying to get property 'NAN' of non-object in %s on line %d

Warning: Division by zero in %sbug76667.php on line %d
