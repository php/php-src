--TEST--
ReflectionMethod class __toString() and export() methods
--FILE--
<?php

function reflectMethod($class, $method) {
    $methodInfo = new ReflectionMethod($class, $method);
    echo "**********************************\n";
    echo "Reflecting on method $class::$method()\n\n";
    echo "__toString():\n";
    var_dump($methodInfo->__toString());
    echo "\nexport():\n";
    var_dump(ReflectionMethod::export($class, $method, true));
    echo "\n**********************************\n";
}

class TestClass
{
    public function foo() {
        echo "Called foo()\n";
    }

    static function stat() {
        echo "Called stat()\n";
    }

    private function priv() {
        echo "Called priv()\n";
    }

    protected function prot() {}

    public function __destruct() {}
}

class DerivedClass extends TestClass {}

interface TestInterface {
    public function int();
}

reflectMethod("DerivedClass", "foo");
reflectMethod("TestClass", "stat");
reflectMethod("TestClass", "priv");
reflectMethod("TestClass", "prot");
reflectMethod("DerivedClass", "prot");
reflectMethod("TestInterface", "int");
reflectMethod("ReflectionProperty", "__construct");
reflectMethod("TestClass", "__destruct");

?>
--EXPECTF--
**********************************
Reflecting on method DerivedClass::foo()

__toString():
string(%d) "Method [ <user, inherits TestClass> public method foo ] {
  @@ %s 16 - 18
}
"

export():
string(%d) "Method [ <user, inherits TestClass> public method foo ] {
  @@ %s 16 - 18
}
"

**********************************
**********************************
Reflecting on method TestClass::stat()

__toString():
string(%d) "Method [ <user> static public method stat ] {
  @@ %s 20 - 22
}
"

export():
string(%d) "Method [ <user> static public method stat ] {
  @@ %s 20 - 22
}
"

**********************************
**********************************
Reflecting on method TestClass::priv()

__toString():
string(%d) "Method [ <user> private method priv ] {
  @@ %s 24 - 26
}
"

export():
string(%d) "Method [ <user> private method priv ] {
  @@ %s 24 - 26
}
"

**********************************
**********************************
Reflecting on method TestClass::prot()

__toString():
string(%d) "Method [ <user> protected method prot ] {
  @@ %s 28 - 28
}
"

export():
string(%d) "Method [ <user> protected method prot ] {
  @@ %s 28 - 28
}
"

**********************************
**********************************
Reflecting on method DerivedClass::prot()

__toString():
string(%d) "Method [ <user, inherits TestClass> protected method prot ] {
  @@ %s 28 - 28
}
"

export():
string(%d) "Method [ <user, inherits TestClass> protected method prot ] {
  @@ %s 28 - 28
}
"

**********************************
**********************************
Reflecting on method TestInterface::int()

__toString():
string(%d) "Method [ <user> abstract public method int ] {
  @@ %s 36 - 36
}
"

export():
string(%d) "Method [ <user> abstract public method int ] {
  @@ %s 36 - 36
}
"

**********************************
**********************************
Reflecting on method ReflectionProperty::__construct()

__toString():
string(%d) "Method [ <internal:Reflection, ctor> public method __construct ] {

  - Parameters [2] {
    Parameter #0 [ <required> $class ]
    Parameter #1 [ <required> $name ]
  }
}
"

export():
string(%d) "Method [ <internal:Reflection, ctor> public method __construct ] {

  - Parameters [2] {
    Parameter #0 [ <required> $class ]
    Parameter #1 [ <required> $name ]
  }
}
"

**********************************
**********************************
Reflecting on method TestClass::__destruct()

__toString():
string(%d) "Method [ <user, dtor> public method __destruct ] {
  @@ %s 30 - 30
}
"

export():
string(%d) "Method [ <user, dtor> public method __destruct ] {
  @@ %s 30 - 30
}
"

**********************************
