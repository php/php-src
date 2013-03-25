--TEST--
ZE2 An abstract method may not be called
--FILE--
<?php

class Root {
}

interface MyInterface
{
	static function MyInterfaceFunc();
}

abstract class Derived extends Root implements MyInterface {
}

class Leaf extends Derived
{
	static function MyInterfaceFunc() {}	
}

var_dump(new Leaf);

class Fails extends Root implements MyInterface {
}

?>
===DONE===
--EXPECTF--
object(Leaf)#%d (0) {
}

Fatal error: Class Fails contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (MyInterface::MyInterfaceFunc) in %sabstract_by_interface_002.php on line %d
