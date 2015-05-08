--TEST--
Bug #68475 Calling function using $callable() syntax with strings of form 'Class::method'
--FILE--
<?php
class TestClass
{
    public $test;

    public function instanceMethod()
    {
        $this->test = "Instance method called.\n";
        echo $this->test;
    }

    public static function staticMethod()
    {
        echo "Static method called!\n";
    }
}

// Test basic call using Class::method syntax.
$callback = 'TestClass::staticMethod';
$callback();

// Case should not matter.
$callback = 'testclass::staticmethod';
$callback();

// Call non-static method.
$callback = 'TestClass::instanceMethod';
try {
    $callback();
} catch (EngineException $e) {
    echo $e->getMessage() . "\n";
}

// Reference undefined method.
$callback = 'TestClass::undefinedMethod';
try {
    $callback();
} catch (EngineException $e) {
    echo $e->getMessage() . "\n";
}

// Reference undefined class.
$callback = 'UndefinedClass::testMethod';
try {
    $callback();
} catch (EngineException $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
Static method called!
Static method called!

Deprecated: Non-static method TestClass::instanceMethod() should not be called statically in %s on line %d
Using $this when not in object context
Call to undefined method TestClass::undefinedMethod()
Class 'UndefinedClass' not found
