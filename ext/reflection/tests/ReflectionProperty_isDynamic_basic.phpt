--TEST--
Test ReflectionProperty::isDynamic() usage.
--FILE--
<?php

function reflectProperty($classOrObj, $property, $className = null) {
    $className ??= $classOrObj;
    $propInfo = new ReflectionProperty($classOrObj, $property);
    echo "**********************************\n";
    echo "Reflecting on property $className::$property\n\n";
    echo "isDynamic():\n";
    var_dump($propInfo->isDynamic());
    echo "\n**********************************\n";
}

#[AllowDynamicProperties]
class TestClass {
    public $pub;
    static public $stat = "static property";
    protected $prot = 4;
    private $priv = "keepOut";
}

reflectProperty("TestClass", "pub");
reflectProperty("TestClass", "stat");
reflectProperty("TestClass", "prot");
reflectProperty("TestClass", "priv");

$obj = new TestClass();
$obj->dyn = 'dynamic';
reflectProperty($obj, "dyn", "TestClass");

?>
--EXPECT--
**********************************
Reflecting on property TestClass::pub

isDynamic():
bool(false)

**********************************
**********************************
Reflecting on property TestClass::stat

isDynamic():
bool(false)

**********************************
**********************************
Reflecting on property TestClass::prot

isDynamic():
bool(false)

**********************************
**********************************
Reflecting on property TestClass::priv

isDynamic():
bool(false)

**********************************
**********************************
Reflecting on property TestClass::dyn

isDynamic():
bool(true)

**********************************
