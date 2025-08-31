--TEST--
Test usage of ReflectionProperty methods isDefault(), isDynamic(), getModifiers(), getDeclaringClass() and getDocComment().
--INI--
opcache.save_comments=1
--FILE--
<?php

function reflectProperty($classOrObj, $property, $className = null) {
    $className ??= $classOrObj;
    $propInfo = new ReflectionProperty($classOrObj, $property);
    echo "**********************************\n";
    echo "Reflecting on property $className::$property\n\n";
    echo "isDefault():\n";
    var_dump($propInfo->isDefault());
    echo "isDynamic():\n";
    var_dump($propInfo->isDynamic());
    echo "getModifiers():\n";
    var_dump($propInfo->getModifiers());
    echo "getDeclaringClass():\n";
    var_dump($propInfo->getDeclaringClass());
    echo "getDocComment():\n";
    var_dump($propInfo->getDocComment());
    echo "\n**********************************\n";
}

#[AllowDynamicProperties]
class TestClass {
    public $pub;
    static public $stat = "static property";
    /**
     * This property has a comment.
     */
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
--EXPECTF--
**********************************
Reflecting on property TestClass::pub

isDefault():
bool(true)
isDynamic():
bool(false)
getModifiers():
int(1)
getDeclaringClass():
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
bool(false)

**********************************
**********************************
Reflecting on property TestClass::stat

isDefault():
bool(true)
isDynamic():
bool(false)
getModifiers():
int(17)
getDeclaringClass():
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
bool(false)

**********************************
**********************************
Reflecting on property TestClass::prot

isDefault():
bool(true)
isDynamic():
bool(false)
getModifiers():
int(2)
getDeclaringClass():
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
string(%d) "/**
     * This property has a comment.
     */"

**********************************
**********************************
Reflecting on property TestClass::priv

isDefault():
bool(true)
isDynamic():
bool(false)
getModifiers():
int(4)
getDeclaringClass():
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
bool(false)

**********************************
**********************************
Reflecting on property TestClass::dyn

isDefault():
bool(false)
isDynamic():
bool(true)
getModifiers():
int(1)
getDeclaringClass():
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(9) "TestClass"
}
getDocComment():
bool(false)

**********************************
