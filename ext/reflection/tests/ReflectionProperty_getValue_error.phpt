--TEST--
Test ReflectionProperty::getValue() errors.
--FILE--
<?php

class TestClass {
    public $pub;
    public $pub2 = 5;
    static public $stat = "static property";
    protected $prot = 4;
    private $priv = "keepOut";
}

class AnotherClass {
}

$instance = new TestClass();
$invalidInstance = new AnotherClass();
$propInfo = new ReflectionProperty('TestClass', 'pub2');

echo "\nInstance without property:\n";
$propInfo = new ReflectionProperty('TestClass', 'stat');

echo "\nStatic property / too many args:\n";
try {
    var_dump($propInfo->getValue($instance, true));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "\nProtected property:\n";
try {
    $propInfo = new ReflectionProperty('TestClass', 'prot');
    var_dump($propInfo->getValue($instance));
}
catch(Exception $exc) {
    echo $exc->getMessage();
}

echo "\n\nInvalid instance:\n";
$propInfo = new ReflectionProperty('TestClass', 'pub2');
try {
    var_dump($propInfo->getValue($invalidInstance));
} catch (ReflectionException $e) {
    echo $e->getMessage();
}

echo "\n\nMissing instance:\n";
try {
    var_dump($propInfo->getValue());
} catch (TypeError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Instance without property:

Static property / too many args:
ReflectionProperty::getValue() expects at most 1 argument, 2 given

Protected property:
Cannot access non-public property TestClass::$prot

Invalid instance:
Given object is not an instance of the class this property was declared in

Missing instance:
ReflectionProperty::getValue(): Argument #1 ($object) must be provided for instance properties
