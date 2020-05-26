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
--EXPECT--
Protected property:
Cannot access non-public property TestClass::$prot

Instance without property:
NULL
string(8) "NewValue"
