--TEST--
ReflectionClass::getName()
--FILE--
<?php
class TrickClass {
	function __toString() {
		//Return the name of another class
		return "Exception";
	}
}

$r1 = new ReflectionClass("StdClass");

$myInstance = new StdClass;
$r2 = new ReflectionClass($myInstance);

$r3 = new ReflectionClass("TrickClass");

var_dump($r1->getName(), $r2->getName(), $r3->getName());

?>
--EXPECT--
string(8) "StdClass"
string(8) "StdClass"
string(10) "TrickClass"
