--TEST--
ReflectionClass::implementsInterface()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
interface I1 {}
class A implements I1 {}
class B extends A {}

interface I2 extends I1 {}
class C implements I2 {}

$classNames = array('A', 'B', 'C', 'I1', 'I2');

foreach ($classNames as $className) {
	$rcs[$className] = new ReflectionClass($className);
}

foreach ($rcs as $childName => $child) {
	foreach ($rcs as $parentName => $parent) {
		echo "Does " . $childName . " implement " . $parentName . "? \n";
		echo "   - Using object argument: ";
		try {
			var_dump($child->implementsInterface($parent));
		} catch (Exception $e) {
			echo $e->getMessage() . "\n";
		}
		echo "   - Using string argument: ";
		try {
			var_dump($child->implementsInterface($parentName));
		} catch (Exception $e) {
			echo $e->getMessage() . "\n";
		}
	}
}



echo "\n\nTest bad arguments:\n";
try {
	var_dump($rcs['A']->implementsInterface());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rcs['A']->implementsInterface('C', 'C'));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rcs['A']->implementsInterface(null));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rcs['A']->implementsInterface('ThisClassDoesNotExist'));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
try {
	var_dump($rcs['A']->implementsInterface(2));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
Does A implement A? 
   - Using object argument: A is not an interface
   - Using string argument: A is not an interface
Does A implement B? 
   - Using object argument: B is not an interface
   - Using string argument: B is not an interface
Does A implement C? 
   - Using object argument: C is not an interface
   - Using string argument: C is not an interface
Does A implement I1? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)
Does A implement I2? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Does B implement A? 
   - Using object argument: A is not an interface
   - Using string argument: A is not an interface
Does B implement B? 
   - Using object argument: B is not an interface
   - Using string argument: B is not an interface
Does B implement C? 
   - Using object argument: C is not an interface
   - Using string argument: C is not an interface
Does B implement I1? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)
Does B implement I2? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Does C implement A? 
   - Using object argument: A is not an interface
   - Using string argument: A is not an interface
Does C implement B? 
   - Using object argument: B is not an interface
   - Using string argument: B is not an interface
Does C implement C? 
   - Using object argument: C is not an interface
   - Using string argument: C is not an interface
Does C implement I1? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)
Does C implement I2? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)
Does I1 implement A? 
   - Using object argument: A is not an interface
   - Using string argument: A is not an interface
Does I1 implement B? 
   - Using object argument: B is not an interface
   - Using string argument: B is not an interface
Does I1 implement C? 
   - Using object argument: C is not an interface
   - Using string argument: C is not an interface
Does I1 implement I1? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)
Does I1 implement I2? 
   - Using object argument: bool(false)
   - Using string argument: bool(false)
Does I2 implement A? 
   - Using object argument: A is not an interface
   - Using string argument: A is not an interface
Does I2 implement B? 
   - Using object argument: B is not an interface
   - Using string argument: B is not an interface
Does I2 implement C? 
   - Using object argument: C is not an interface
   - Using string argument: C is not an interface
Does I2 implement I1? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)
Does I2 implement I2? 
   - Using object argument: bool(true)
   - Using string argument: bool(true)


Test bad arguments:

Warning: ReflectionClass::implementsInterface() expects exactly 1 parameter, 0 given in %s on line 37
NULL

Warning: ReflectionClass::implementsInterface() expects exactly 1 parameter, 2 given in %s on line 42
NULL
Parameter one must either be a string or a ReflectionClass object
Interface ThisClassDoesNotExist does not exist
Parameter one must either be a string or a ReflectionClass object
