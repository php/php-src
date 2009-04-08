--TEST--
Test ReflectionProperty::setValue() error cases.
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
var_dump($propInfo->setValue());
var_dump($propInfo->setValue($instance));

echo "\nToo many args:\n";
var_dump($propInfo->setValue($instance, "NewValue", true));

echo "\nWrong type of arg:\n";
var_dump($propInfo->setValue(true, "NewValue"));
$propInfo = new ReflectionProperty('TestClass', 'stat');

echo "\nStatic property / too many args:\n";
var_dump($propInfo->setValue($instance, "NewValue", true));

echo "\nStatic property / too few args:\n";
var_dump($propInfo->setValue("A new value"));
var_dump(TestClass::$stat);
var_dump($propInfo->setValue());
var_dump(TestClass::$stat);

echo "\nStatic property / wrong type of arg:\n";
var_dump($propInfo->setValue(true, "Another new value"));
var_dump(TestClass::$stat);

echo "\nProtected property:\n";
try {
    $propInfo = new ReflectionProperty('TestClass', 'prot');
    var_dump($propInfo->setValue($instance, "NewValue"));
}
catch(Exception $exc) {
    echo $exc->getMessage();
}

echo "\n\nInstance without property:\n";
$propInfo = new ReflectionProperty('TestClass', 'pub2');
var_dump($propInfo->setValue($instanceWithNoProperties, "NewValue"));
var_dump($instanceWithNoProperties->pub2);
?>
--EXPECTF--
Too few args:

Warning: ReflectionProperty::setValue() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: ReflectionProperty::setValue() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Too many args:

Warning: ReflectionProperty::setValue() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Wrong type of arg:

Warning: ReflectionProperty::setValue() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Static property / too many args:

Warning: ReflectionProperty::setValue() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Static property / too few args:
NULL
string(11) "A new value"

Warning: ReflectionProperty::setValue() expects exactly 2 parameters, 0 given in %s on line %d
NULL
string(11) "A new value"

Static property / wrong type of arg:
NULL
string(17) "Another new value"

Protected property:
Cannot access non-public member TestClass::prot

Instance without property:
NULL
string(8) "NewValue"
