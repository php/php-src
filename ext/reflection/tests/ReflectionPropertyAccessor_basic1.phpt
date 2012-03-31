--TEST--
Test usage of ReflectionPropertyAccessor methods __toString(), export(), getName(), isPublic(), isPrivate(), isProtected(), isStatic(), getValue() and setValue().
--FILE--
<?php

function reflectProperty($class, $property) {
    $propInfo = new ReflectionPropertyAccessor($class, $property);
    echo "**********************************\n";
    echo "Reflecting on property $class::$property\n\n";
    echo "__toString():\n";
    var_dump($propInfo->__toString());
    echo "export():\n";
    var_dump(ReflectionPropertyAccessor::export($class, $property, true));
    echo "export():\n";
    var_dump(ReflectionPropertyAccessor::export($class, $property, false));
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
    echo "isReadOnly():\n";
    var_dump($propInfo->isReadOnly());
    echo "isWriteOnly():\n";
    var_dump($propInfo->isWriteOnly());
    echo "isDefault():\n";
    var_dump($propInfo->isDefault());
    
    $instance = new $class();
    if ($propInfo->getGetter() != false && $propInfo->getGetter()->isPublic()) {
        echo "getValue():\n";
        var_dump($propInfo->getValue($instance));
    }
    if ($propInfo->getSetter() != false && $propInfo->getSetter()->isPublic()) {
        echo "setValue(\"NewValue\"):\n";
        $propInfo->setValue($instance, "NewValue");
        if($propInfo->getGetter() != false) {
	        echo "getValue() after a setValue():\n";
	        var_dump($propInfo->getValue($instance));
	    }
    }
    echo "\n**********************************\n";
}
	
class TestClass {
	private $_a1 = 'b1', $_a2 = 'b2', $_a3 = 'b3', $_a4 = 'b4';
	
	public read-only $a1 {
		get { return $this->_a1; }
	}
	public write-only $a2 {
		set { $this->_a2 = $value; }
	}
	public $a3 {
		get { return $this->_a3; }
		set { $this->_a3 = $value; }
	}
	public static $a4 {
		protected get { return $this->_a4; }
		private set { $this->a5 = $value; }
	}
}
reflectProperty("TestClass", "a1");
reflectProperty("TestClass", "a2");
reflectProperty("TestClass", "a3");
reflectProperty("TestClass", "a4");

?>
--EXPECTF--
**********************************
Reflecting on property TestClass::a1

__toString():
string(%d) "Accessor [ public read-only $a1 ] {
    Method [ <user> public method __geta1 ] {
      @@ %s 50 - 50
    }
}
 ]
"
export():
string(%d) "Accessor [ public read-only $a1 ] {
    Method [ <user> public method __geta1 ] {
      @@ %s 50 - 50
    }
}
 ]
"
export():
Accessor [ public read-only $a1 ] {
    Method [ <user> public method __geta1 ] {
      @@ %s 50 - 50
    }
}
 ]

NULL
getName():
string(2) "a1"
isPublic():
bool(true)
isPrivate():
bool(false)
isProtected():
bool(false)
isStatic():
bool(false)
isReadOnly():
bool(true)
isWriteOnly():
bool(false)
isDefault():
bool(true)
getValue():
string(2) "b1"

**********************************
**********************************
Reflecting on property TestClass::a2

__toString():
string(%d) "Accessor [ public write-only $a2 ] {
    Method [ <user> public method __seta2 ] {
      @@ %s 53 - 53

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
}
 ]
"
export():
string(%d) "Accessor [ public write-only $a2 ] {
    Method [ <user> public method __seta2 ] {
      @@ %s 53 - 53

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
}
 ]
"
export():
Accessor [ public write-only $a2 ] {
    Method [ <user> public method __seta2 ] {
      @@ %s 53 - 53

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
}
 ]

NULL
getName():
string(2) "a2"
isPublic():
bool(true)
isPrivate():
bool(false)
isProtected():
bool(false)
isStatic():
bool(false)
isReadOnly():
bool(false)
isWriteOnly():
bool(true)
isDefault():
bool(true)
setValue("NewValue"):

**********************************
**********************************
Reflecting on property TestClass::a3

__toString():
string(%d) "Accessor [ public $a3 ] {
    Method [ <user> public method __geta3 ] {
      @@ %s 56 - 56
    }
    Method [ <user> public method __seta3 ] {
      @@ %s 57 - 57

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
}
 ]
"
export():
string(%d) "Accessor [ public $a3 ] {
    Method [ <user> public method __geta3 ] {
      @@ %s 56 - 56
    }
    Method [ <user> public method __seta3 ] {
      @@ %s 57 - 57

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
}
 ]
"
export():
Accessor [ public $a3 ] {
    Method [ <user> public method __geta3 ] {
      @@ %s 56 - 56
    }
    Method [ <user> public method __seta3 ] {
      @@ %s 57 - 57

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
}
 ]

NULL
getName():
string(2) "a3"
isPublic():
bool(true)
isPrivate():
bool(false)
isProtected():
bool(false)
isStatic():
bool(false)
isReadOnly():
bool(false)
isWriteOnly():
bool(false)
isDefault():
bool(true)
getValue():
string(2) "b3"
setValue("NewValue"):
getValue() after a setValue():
string(8) "NewValue"

**********************************
**********************************
Reflecting on property TestClass::a4

__toString():
string(%d) "Accessor [ public static $a4 ] {
    Method [ <user> static protected method __geta4 ] {
      @@ %s 60 - 60
    }
    Method [ <user> static private method __seta4 ] {
      @@ %s 61 - 61

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
}
 ]
"
export():
string(%d) "Accessor [ public static $a4 ] {
    Method [ <user> static protected method __geta4 ] {
      @@ %s 60 - 60
    }
    Method [ <user> static private method __seta4 ] {
      @@ %s 61 - 61

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
}
 ]
"
export():
Accessor [ public static $a4 ] {
    Method [ <user> static protected method __geta4 ] {
      @@ %s 60 - 60
    }
    Method [ <user> static private method __seta4 ] {
      @@ %s 61 - 61

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
}
 ]

NULL
getName():
string(2) "a4"
isPublic():
bool(true)
isPrivate():
bool(false)
isProtected():
bool(false)
isStatic():
bool(true)
isReadOnly():
bool(false)
isWriteOnly():
bool(false)
isDefault():
bool(true)

**********************************