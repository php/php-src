--TEST--
ReflectionParameter::__construct(): Invalid method as constructor
--FILE--
<?php

// Invalid class name
try {
	new ReflectionParameter (array ('A', 'b'), 0);
} catch (ReflectionException $e) { echo $e->getMessage()."\n"; }

// Invalid class method
try {
	new ReflectionParameter (array ('C', 'b'), 0);
} catch (ReflectionException $e) { echo $e->getMessage ()."\n"; }

// Invalid object method
try {
	new ReflectionParameter (array (new C, 'b'), 0);
} catch (ReflectionException $e) { echo $e->getMessage ()."\n"; }


class C {
}

try {
	new ReflectionParameter(array ('A', 'b'));
}
catch(TypeError $e) {
	printf( "Ok - %s\n", $e->getMessage());
}

try {
	new ReflectionParameter(0, 0);
}
catch(ReflectionException $e) {
	printf( "Ok - %s\n", $e->getMessage());
}

echo "Done.\n";

?>
--EXPECTF--
Class A does not exist
Method C::b() does not exist
Method C::b() does not exist
Ok - ReflectionParameter::__construct() expects exactly 2 parameters, 1 given
Ok - The parameter class is expected to be either a string, an array(class, method) or a callable object
Done.
