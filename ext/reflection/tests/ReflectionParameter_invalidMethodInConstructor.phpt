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
--EXPECT--
Class "A" does not exist
Method C::b() does not exist
Method C::b() does not exist
Ok - ReflectionParameter::__construct() expects exactly 2 arguments, 1 given
Ok - ReflectionParameter::__construct(): Argument #1 ($function) must be a string, an array(class, method), or a callable object, int given
Done.
