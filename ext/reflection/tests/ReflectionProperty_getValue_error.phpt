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
var_dump($propInfo->getValue($instance, true));

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
var_dump($propInfo->getValue($invalidInstance));

?>
--EXPECTF--
Instance without property:

Static property / too many args:
string(15) "static property"

Protected property:
Cannot access non-public member TestClass::$prot

Invalid instance:

Fatal error: Uncaught ReflectionException: Given object is not an instance of the class this property was declared in in %s:%d
Stack trace:
#0 %s(%d): ReflectionProperty->getValue(Object(AnotherClass))
#1 {main}
  thrown in %s on line %d
