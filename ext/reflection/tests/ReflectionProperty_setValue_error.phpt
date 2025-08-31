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

#[AllowDynamicProperties]
class AnotherClass {
}

$instance = new TestClass();
$instanceWithNoProperties = new AnotherClass();
$propInfo = new ReflectionProperty('TestClass', 'pub2');

echo "\nProtected property:\n";

$propInfo = new ReflectionProperty('TestClass', 'prot');
$propInfo->setValue($instance, "NewValue");
var_dump($propInfo->getValue($instance));

echo "\n\nInstance without property:\n";
$propInfo = new ReflectionProperty('TestClass', 'pub2');
var_dump($propInfo->setValue($instanceWithNoProperties, "NewValue"));
var_dump($instanceWithNoProperties->pub2);
?>
--EXPECT--
Protected property:
string(8) "NewValue"


Instance without property:
NULL
string(8) "NewValue"
