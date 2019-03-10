--TEST--
A locked class may still have __set overload
--FILE--
<?php

locked class TestClass {
	public $definedProp;
	private $privateProp;

	public function __set($prop, $value) {
		echo "__set called for $prop with '$value'\n";
	}
}

$t = new testClass();

$t->definedProp = "OK\n";
echo $t->definedProp;

$t->privateProp = "call magic";
$t->nonExistentProp = "call magic again";

echo "Done\n";
?>
--EXPECT--
OK
__set called for privateProp with 'call magic'
__set called for nonExistentProp with 'call magic again'
Done

