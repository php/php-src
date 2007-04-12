--TEST--
Bug #38653 (memory leak in ReflectionClass::getConstant())
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class foo {
	    const cons = 10;
	    const cons1 = "";
	    const cons2 = "test";
}

class bar extends foo {
}

$foo = new ReflectionClass("foo");
var_dump($foo->getConstant("cons"));
var_dump($foo->getConstant("cons1"));
var_dump($foo->getConstant("cons2"));
var_dump($foo->getConstant("no such const"));

echo "Done\n";
?>
--EXPECTF--	
int(10)
string(0) ""
string(4) "test"
bool(false)
Done
