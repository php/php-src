--TEST--
Test usage of ReflectionProperty methods __toString(), export(), getName(), isPublic(), isPrivate(), isProtected(), isStatic(), getValue() and setValue().
--FILE--
<?php

function reflectProperty($class, $property) {
    $propInfo = new ReflectionProperty($class, $property);
    echo "**********************************\n";
    echo "Reflecting on property $class::$property\n\n";
    echo "__toString():\n";
    var_dump($propInfo->__toString());
    echo "export():\n";
    var_dump(ReflectionProperty::export($class, $property, true));
    echo "export():\n";
    var_dump(ReflectionProperty::export($class, $property, false));
    echo "getName():\n";
    var_dump($propInfo->getName());
    echo "isPublic():\n";
    var_dump($propInfo->isPublic());
    echo "isPrivate():\n";
    var_dump($propInfo->isPrivate());
    echo "isProtected():\n";
    var_dump($propInfo->isProtected());
    echo "isStatic():\n";
    var_dump($propInfo->isStatic());
    $instance = new $class();
    if ($propInfo->isPublic()) {
        echo "getValue():\n";
        var_dump($propInfo->getValue($instance));
        $propInfo->setValue($instance, "NewValue");
        echo "getValue() after a setValue():\n";
        var_dump($propInfo->getValue($instance));
    }
    echo "\n**********************************\n";
}

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

?>
--EXPECTF--
**********************************
Reflecting on property TestClass::pub

__toString():
string(35) "Property [ <default> public $pub ]
"
export():

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
string(35) "Property [ <default> public $pub ]
"
export():

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
Property [ <default> public $pub ]

NULL
getName():
string(3) "pub"
isPublic():
bool(true)
isPrivate():
bool(false)
isProtected():
bool(false)
isStatic():
bool(false)
getValue():
NULL
getValue() after a setValue():
string(8) "NewValue"

**********************************
**********************************
Reflecting on property TestClass::stat

__toString():
string(33) "Property [ public static $stat ]
"
export():

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
string(33) "Property [ public static $stat ]
"
export():

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
Property [ public static $stat ]

NULL
getName():
string(4) "stat"
isPublic():
bool(true)
isPrivate():
bool(false)
isProtected():
bool(false)
isStatic():
bool(true)
getValue():
string(15) "static property"
getValue() after a setValue():
string(8) "NewValue"

**********************************
**********************************
Reflecting on property TestClass::prot

__toString():
string(39) "Property [ <default> protected $prot ]
"
export():

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
string(39) "Property [ <default> protected $prot ]
"
export():

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
Property [ <default> protected $prot ]

NULL
getName():
string(4) "prot"
isPublic():
bool(false)
isPrivate():
bool(false)
isProtected():
bool(true)
isStatic():
bool(false)

**********************************
**********************************
Reflecting on property TestClass::priv

__toString():
string(37) "Property [ <default> private $priv ]
"
export():

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
string(37) "Property [ <default> private $priv ]
"
export():

Deprecated: Function ReflectionProperty::export() is deprecated in %s on line %d
Property [ <default> private $priv ]

NULL
getName():
string(4) "priv"
isPublic():
bool(false)
isPrivate():
bool(true)
isProtected():
bool(false)
isStatic():
bool(false)

**********************************
