--TEST--
Indirect method call by array - Calling __call() and __callStatic()
--FILE--
<?php

class foo {
	public function __call($a, $b) {
		printf("From %s:\n", __METHOD__);
		var_dump($a);
		var_dump($this);
	}
	static public function __callStatic($a, $b) {
		printf("From %s:\n", __METHOD__);
		var_dump($a);
		var_dump($this);
	}
}

$arr = array('foo', 'abc');
try {
	$arr();
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
$foo = new foo;
$arr = array($foo, 'abc');
$arr();


?>
--EXPECTF--
From foo::__callStatic:
string(3) "abc"
Exception: Using $this when not in object context
From foo::__call:
string(3) "abc"
object(foo)#%d (0) {
}
