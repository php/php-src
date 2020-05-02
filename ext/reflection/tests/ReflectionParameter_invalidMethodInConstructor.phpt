--TEST--
ReflectionParameter::__construct(): Invalid method as constructor
--FILE--
<?php

// Invalid class name
try {
    new ReflectionParameter (array ('A', 'b'), 0);
} catch (\Throwable $e) { echo get_class($e) . ': ' . $e->getMessage() . "\n"; }

// Invalid class method
try {
    new ReflectionParameter (array ('C', 'b'), 0);
} catch (\Throwable $e) { echo get_class($e) . ': ' . $e->getMessage() . "\n"; }

// Invalid object method
try {
    new ReflectionParameter (array (new C, 'b'), 0);
} catch (\Throwable $e) { echo get_class($e) . ': ' . $e->getMessage() . "\n"; }


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
} catch (\Throwable $e) { echo get_class($e) . ': ' . $e->getMessage() . "\n"; }

echo "Done.\n";

?>
--EXPECT--
TypeError: ReflectionParameter::__construct(): Argument #1 ($function) must be a valid callback, class 'A' not found
TypeError: ReflectionParameter::__construct(): Argument #1 ($function) must be a valid callback, class 'C' does not have a method 'b'
TypeError: ReflectionParameter::__construct(): Argument #1 ($function) must be a valid callback, class 'C' does not have a method 'b'
Ok - ReflectionParameter::__construct() expects exactly 2 parameters, 1 given
TypeError: ReflectionParameter::__construct(): Argument #1 ($function) must be a valid callback, no array or string given
Done.
