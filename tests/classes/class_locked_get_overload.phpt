--TEST--
A locked class may still have __get overload
--FILE--
<?php

locked class TestClass {
	public $definedProp;
	private $privateProp;

	public function __get($prop) {
		return "__get called for $prop\n";
	}
}

$t = new testClass();

$t->definedProp = "OK\n";
echo $t->definedProp;

echo $t->privateProp;
echo $t->nonExistentProp;

echo "Done\n";
?>
--EXPECT--
OK
__get called for privateProp
__get called for nonExistentProp
Done

