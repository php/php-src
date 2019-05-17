--TEST--
Testing direct assigning for property of object returned by function
--FILE--
<?php

class foo {
	static $bar = array();

	public function __set($a, $b) {
		self::$bar[] = $b;
	}

	public function __get($a) {
		/* last */
		return self::$bar[count(self::$bar)-1];
	}
}

function test() {
	return new foo;
}

$a = test()->bar = 1;
var_dump($a, count(foo::$bar), test()->whatever);

print "\n";

$a = test()->bar = NULL;
var_dump($a, count(foo::$bar), test()->whatever);

print "\n";

$a = test()->bar = test();
var_dump($a, count(foo::$bar), test()->whatever);

print "\n";

?>
--EXPECTF--
int(1)
int(1)
int(1)

NULL
int(2)
NULL

object(foo)#%d (0) {
}
int(3)
object(foo)#%d (0) {
}
