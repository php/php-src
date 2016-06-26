--TEST--
Bug #38217 (ReflectionClass::newInstanceArgs() tries to allocate too much memory)
--FILE--
<?php

class Object {
	public function __construct() {
	}
}

$class= new ReflectionClass('Object');
var_dump($class->newInstanceArgs());

class Object1 {
	public function __construct($var) {
		var_dump($var);
	}
}

$class= new ReflectionClass('Object1');
try {
	var_dump($class->newInstanceArgs());
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
var_dump($class->newInstanceArgs(array('test')));


echo "Done\n";
?>
--EXPECTF--	
object(Object)#%d (0) {
}
Exception: Too few arguments to function Object1::__construct(), 0 passed and exactly 1 expected
string(4) "test"
object(Object1)#%d (0) {
}
Done
