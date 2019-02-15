--TEST--
ReflectionClass::isSubclassOf()
--FILE--
<?php
class A {}
class B extends A {}
class C extends B {}

interface I {}
class X implements I {}

$classNames = array('A', 'B', 'C', 'I', 'X');

foreach ($classNames as $className) {
	$rcs[$className] = new ReflectionClass($className);
}

foreach ($rcs as $childName => $child) {
	foreach ($rcs as $parentName => $parent) {
		echo "Is " . $childName . " a subclass of " . $parentName . "? \n";
		echo "   - Using object argument: ";
		var_dump($child->isSubclassOf($parent));
		echo "   - Using string argument: ";
		var_dump($child->isSubclassOf($parentName));
	}
}
?>
--EXPECT--
Is A a subclass of A? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is A a subclass of B? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is A a subclass of C? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is A a subclass of I? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is A a subclass of X? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is B a subclass of A? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)
Is B a subclass of B? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is B a subclass of C? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is B a subclass of I? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is B a subclass of X? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is C a subclass of A? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)
Is C a subclass of B? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)
Is C a subclass of C? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is C a subclass of I? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is C a subclass of X? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is I a subclass of A? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is I a subclass of B? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is I a subclass of C? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is I a subclass of I? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is I a subclass of X? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is X a subclass of A? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is X a subclass of B? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is X a subclass of C? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Is X a subclass of I? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)
Is X a subclass of X? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
