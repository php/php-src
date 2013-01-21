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
    echo "isDefault():\n";
    var_dump($propInfo->isDefault());
    
    $instance = new $class();
    if ($propInfo->getGet() != false && $propInfo->getGet()->isPublic()) {
        echo "getValue():\n";
        var_dump($propInfo->getValue($instance));
    }
    if ($propInfo->getSet() != false && $propInfo->getSet()->isPublic()) {
        echo "setValue(\"NewValue\"):\n";
        $propInfo->setValue($instance, "NewValue");
        if($propInfo->getGet() != false) {
	        echo "getValue() after a setValue():\n";
	        var_dump($propInfo->getValue($instance));
	    }
    }
    echo "\n**********************************\n";
}
	
class TestClass {
	private $_a1 = 'b1', $_a2 = 'b2', $_a3 = 'b3', $_a4 = 'b4';
	
	public $a1 {
		get { echo __METHOD__.'() = '; return $this->_a1; }
	}
	public $a2 {
		set { echo __METHOD__."($value)".PHP_EOL; $this->_a2 = $value; }
	}
	public $a3 {
		get { echo __METHOD__.'() = '; return $this->_a3; }
		set { echo __METHOD__."($value)".PHP_EOL; $this->_a3 = $value; }
	}
/*	Commented out until static accessors are supported
	public static $a4 {
		protected get { return $this->_a4; }
		private set { $this->a5 = $value; }
	}
*/
}
reflectProperty("TestClass", "a1");
reflectProperty("TestClass", "a2");
reflectProperty("TestClass", "a3");
/*	Commented out until static accessors are supported
reflectProperty("TestClass", "a4");
*/

?>
--EXPECTF--
**********************************
Reflecting on property TestClass::a1

__toString():
string(%d) "Accessor [ public $a1 ] {
    Method [ <user> public method $a1->get ] {
      @@ %s %d - %d
    }
    Method [ <user> public method $a1->isset ] {
      @@ %s %d - %d
    }
}
 ]
"
export():
string(%d) "Accessor [ public $a1 ] {
    Method [ <user> public method $a1->get ] {
      @@ %s %d - %d
    }
    Method [ <user> public method $a1->isset ] {
      @@ %s %d - %d
    }
}
 ]
"
export():
Accessor [ public $a1 ] {
    Method [ <user> public method $a1->get ] {
      @@ %s %d - %d
    }
    Method [ <user> public method $a1->isset ] {
      @@ %s %d - %d
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
isDefault():
bool(true)
getValue():
TestClass::$a1->get() = string(2) "b1"

**********************************
**********************************
Reflecting on property TestClass::a2

__toString():
string(%d) "Accessor [ public $a2 ] {
    Method [ <user> public method $a2->set ] {
      @@ %s %d - %d

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
    Method [ <user> public method $a2->unset ] {
      @@ %s %d - %d
    }
}
 ]
"
export():
string(%d) "Accessor [ public $a2 ] {
    Method [ <user> public method $a2->set ] {
      @@ %s %d - %d

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
    Method [ <user> public method $a2->unset ] {
      @@ %s %d - %d
    }
}
 ]
"
export():
Accessor [ public $a2 ] {
    Method [ <user> public method $a2->set ] {
      @@ %s %d - %d

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
    Method [ <user> public method $a2->unset ] {
      @@ %s %d - %d
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
isDefault():
bool(true)
setValue("NewValue"):
TestClass::$a2->set(NewValue)

**********************************
**********************************
Reflecting on property TestClass::a3

__toString():
string(%d) "Accessor [ public $a3 ] {
    Method [ <user> public method $a3->get ] {
      @@ %s %d - %d
    }
    Method [ <user> public method $a3->set ] {
      @@ %s %d - %d

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
    Method [ <user> public method $a3->isset ] {
      @@ %s %d - %d
    }
    Method [ <user> public method $a3->unset ] {
      @@ %s %d - %d
    }
}
 ]
"
export():
string(%d) "Accessor [ public $a3 ] {
    Method [ <user> public method $a3->get ] {
      @@ %s %d - %d
    }
    Method [ <user> public method $a3->set ] {
      @@ %s %d - %d

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
    Method [ <user> public method $a3->isset ] {
      @@ %s %d - %d
    }
    Method [ <user> public method $a3->unset ] {
      @@ %s %d - %d
    }
}
 ]
"
export():
Accessor [ public $a3 ] {
    Method [ <user> public method $a3->get ] {
      @@ %s %d - %d
    }
    Method [ <user> public method $a3->set ] {
      @@ %s %d - %d

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }
    Method [ <user> public method $a3->isset ] {
      @@ %s %d - %d
    }
    Method [ <user> public method $a3->unset ] {
      @@ %s %d - %d
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
isDefault():
bool(true)
getValue():
TestClass::$a3->get() = string(2) "b3"
setValue("NewValue"):
TestClass::$a3->set(NewValue)
getValue() after a setValue():
TestClass::$a3->get() = string(8) "NewValue"

**********************************
