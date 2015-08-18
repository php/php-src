--TEST--
Bug #68475 Calling function using $callable() syntax with strings of form 'Class::method'
--FILE--
<?php
class TestClass
{
    public static function staticMethod()
    {
        echo "Static method called!\n";
    }

    public static function staticMethodWithArgs($arg1, $arg2, $arg3)
    {
        printf("Static method called with args: %s, %s, %s\n", $arg1, $arg2, $arg3);
    }
}

// Test basic call using Class::method syntax.
$callback = 'TestClass::staticMethod';
$callback();

// Case should not matter.
$callback = 'testclass::staticmethod';
$callback();

$args = ['arg1', 'arg2', 'arg3'];
$callback = 'TestClass::staticMethodWithArgs';

// Test call with args.
$callback($args[0], $args[1], $args[2]);

// Test call with splat operator.
$callback(...$args);

// Reference undefined method.
$callback = 'TestClass::undefinedMethod';
try {
    $callback();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

// Reference undefined class.
$callback = 'UndefinedClass::testMethod';
try {
    $callback();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Static method called!
Static method called!
Static method called with args: arg1, arg2, arg3
Static method called with args: arg1, arg2, arg3
Call to undefined method TestClass::undefinedMethod()
Class 'UndefinedClass' not found
