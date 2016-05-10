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
$instanceWithNoProperties = new AnotherClass();
$propInfo = new ReflectionProperty('TestClass', 'pub2');

echo "Too few args:\n";
var_dump($propInfo->getValue());

echo "\nToo many args:\n";
var_dump($propInfo->getValue($instance, true));

echo "\nWrong type of arg:\n";
var_dump($propInfo->getValue(true));

echo "\nInstance without property:\n";
$propInfo = new ReflectionProperty('TestClass', 'stat');

echo "\nStatic property / too many args:\n";
var_dump($propInfo->getValue($instance, true));

echo "\nStatic property / wrong type of arg:\n";
var_dump($propInfo->getValue(true));

echo "\nProtected property:\n";
try {
    $propInfo = new ReflectionProperty('TestClass', 'prot');
    var_dump($propInfo->getValue($instance));
}
catch(Exception $exc) {
    echo $exc->getMessage();
}

echo "\n\nInstance without property:\n";
$propInfo = new ReflectionProperty('TestClass', 'pub2');
var_dump($propInfo->getValue($instanceWithNoProperties));

?>
--EXPECTF--
Too few args:

Warning: ReflectionProperty::getValue() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Too many args:

Warning: ReflectionProperty::getValue() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Wrong type of arg:

Warning: ReflectionProperty::getValue() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Instance without property:

Static property / too many args:
string(15) "static property"

Static property / wrong type of arg:
string(15) "static property"

Protected property:
Cannot access non-public member TestClass::prot

Instance without property:

Notice: Undefined property: AnotherClass::$pub2 in %s on line %d
NULL
