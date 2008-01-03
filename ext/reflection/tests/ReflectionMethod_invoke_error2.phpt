--TEST--
ReflectionMethod::invoke() further errors
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class TestClass {

    public function methodWithArgs($a, $b) {
        echo "Called methodWithArgs($a, $b)\n";
    }
}

$methodWithArgs = new ReflectionMethod('TestClass', 'methodWithArgs');

$testClassInstance = new TestClass();

echo "\nMethod with args:\n";
var_dump($methodWithArgs->invoke($testClassInstance));

?>
--EXPECTF--
Method with args:

Warning: Missing argument 1 for TestClass::methodWithArgs() in %s on line %d

Warning: Missing argument 2 for TestClass::methodWithArgs() in %s on line %d

Notice: Undefined variable: a in %s on line %d

Notice: Undefined variable: b in %s on line %d
Called methodWithArgs(, )
NULL
