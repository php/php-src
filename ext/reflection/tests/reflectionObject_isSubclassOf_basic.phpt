--TEST--
ReflectionObject::isSubclassOf() - basic function test
--FILE--
<?php
class A {}
class B extends A {}
class C extends B {}

interface I {}
class X implements I {}

$classNames = array('A', 'B', 'C', 'I', 'X'); 

//Create ReflectionClasses
foreach ($classNames as $className) {
	$rcs[$className] = new ReflectionClass($className);
}

//Create ReflectionObjects
foreach ($classNames as $className) {
	if ($rcs[$className]->isInstantiable()) {
		$ros[$className] = new ReflectionObject(new $className);
	}
}

foreach ($ros as $childName => $child) {
	foreach ($rcs as $parentName => $parent) {
		echo "Is " . $childName . " a subclass of " . $parentName . "? \n";
		echo "   - Using ReflectionClass object argument: ";
		var_dump($child->isSubclassOf($parent));
		if ($parent->isInstantiable()) {
			echo "   - Using ReflectionObject object argument: ";
			var_dump($child->isSubclassOf($ros[$parentName]));
		}
		echo "   - Using string argument: ";
		var_dump($child->isSubclassOf($parentName)); 
	}
}
?>
--EXPECTF--
Is A a subclass of A? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is A a subclass of B? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is A a subclass of C? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is A a subclass of I? 
   - Using ReflectionClass object argument: bool(false)
   - Using string argument: bool(false)
Is A a subclass of X? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is B a subclass of A? 
   - Using ReflectionClass object argument: bool(true)
   - Using ReflectionObject object argument: bool(true)
   - Using string argument: bool(true)
Is B a subclass of B? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is B a subclass of C? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is B a subclass of I? 
   - Using ReflectionClass object argument: bool(false)
   - Using string argument: bool(false)
Is B a subclass of X? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is C a subclass of A? 
   - Using ReflectionClass object argument: bool(true)
   - Using ReflectionObject object argument: bool(true)
   - Using string argument: bool(true)
Is C a subclass of B? 
   - Using ReflectionClass object argument: bool(true)
   - Using ReflectionObject object argument: bool(true)
   - Using string argument: bool(true)
Is C a subclass of C? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is C a subclass of I? 
   - Using ReflectionClass object argument: bool(false)
   - Using string argument: bool(false)
Is C a subclass of X? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is X a subclass of A? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is X a subclass of B? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is X a subclass of C? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
Is X a subclass of I? 
   - Using ReflectionClass object argument: bool(true)
   - Using string argument: bool(true)
Is X a subclass of X? 
   - Using ReflectionClass object argument: bool(false)
   - Using ReflectionObject object argument: bool(false)
   - Using string argument: bool(false)
