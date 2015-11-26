--TEST--
ReflectionMethod class getName(), isInternal() and isUserDefined() methods
--FILE--
<?php

function reflectMethod($class, $method) {
    $methodInfo = new ReflectionMethod($class, $method);
    echo "**********************************\n";
    echo "Reflecting on method $class::$method()\n\n";
    echo "\ngetName():\n";
    var_dump($methodInfo->getName());
    echo "\nisInternal():\n";
    var_dump($methodInfo->isInternal());
    echo "\nisUserDefined():\n";
    var_dump($methodInfo->isUserDefined());
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
--EXPECT--
**********************************
Reflecting on method DerivedClass::foo()


getName():
string(3) "foo"

isInternal():
bool(false)

isUserDefined():
bool(true)

**********************************
**********************************
Reflecting on method TestClass::stat()


getName():
string(4) "stat"

isInternal():
bool(false)

isUserDefined():
bool(true)

**********************************
**********************************
Reflecting on method TestClass::priv()


getName():
string(4) "priv"

isInternal():
bool(false)

isUserDefined():
bool(true)

**********************************
**********************************
Reflecting on method TestClass::prot()


getName():
string(4) "prot"

isInternal():
bool(false)

isUserDefined():
bool(true)

**********************************
**********************************
Reflecting on method DerivedClass::prot()


getName():
string(4) "prot"

isInternal():
bool(false)

isUserDefined():
bool(true)

**********************************
**********************************
Reflecting on method TestInterface::int()


getName():
string(3) "int"

isInternal():
bool(false)

isUserDefined():
bool(true)

**********************************
**********************************
Reflecting on method ReflectionProperty::__construct()


getName():
string(11) "__construct"

isInternal():
bool(true)

isUserDefined():
bool(false)

**********************************
**********************************
Reflecting on method TestClass::__destruct()


getName():
string(10) "__destruct"

isInternal():
bool(false)

isUserDefined():
bool(true)

**********************************
