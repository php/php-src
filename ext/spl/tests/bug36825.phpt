--TEST--
Bug #36825 ()
--FILE--
<?php

class foo extends ArrayObject {
	public function __construct() {
	}

	public function offsetGet($key) {
		throw new Exception("hi");
	}
}

$test=new foo();
try{
	var_dump($test['bar']);
} catch (Exception $e) {
	echo "got exception\n";
}

echo "Done\n";
?>
--EXPECT--	
got exception
Done
