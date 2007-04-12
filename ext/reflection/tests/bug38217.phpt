--TEST--
Bug #38217 (ReflectionClass::newInstanceArgs() tries to allocate too much memory)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
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
var_dump($class->newInstanceArgs());
var_dump($class->newInstanceArgs(array('test')));


echo "Done\n";
?>
--EXPECTF--	
object(Object)#%d (0) {
}

Warning: Missing argument 1 for Object1::__construct() in %s on line %d

Notice: Undefined variable: var in %s on line %d
NULL
object(Object1)#%d (0) {
}
string(4) "test"
object(Object1)#%d (0) {
}
Done
