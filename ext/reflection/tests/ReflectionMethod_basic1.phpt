--TEST--
ReflectionMethod class - various methods
--FILE--
<?php

function reflectMethod($class, $method) {
    $methodInfo = new ReflectionMethod($class, $method);
    echo "**********************************\n";
    echo "Reflecting on method $class::$method()\n\n";
    echo "\nisFinal():\n";
    var_dump($methodInfo->isFinal());
    echo "\nisAbstract():\n";
    var_dump($methodInfo->isAbstract());
    echo "\nisPublic():\n";
    var_dump($methodInfo->isPublic());
    echo "\nisPrivate():\n";
    var_dump($methodInfo->isPrivate());
    echo "\nisProtected():\n";
    var_dump($methodInfo->isProtected());
    echo "\nisStatic():\n";
    var_dump($methodInfo->isStatic());
    echo "\nisConstructor():\n";
    var_dump($methodInfo->isConstructor());
    echo "\nisDestructor():\n";
    var_dump($methodInfo->isDestructor());
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


isFinal():
bool(false)

isAbstract():
bool(false)

isPublic():
bool(true)

isPrivate():
bool(false)

isProtected():
bool(false)

isStatic():
bool(false)

isConstructor():
bool(false)

isDestructor():
bool(false)

**********************************
**********************************
Reflecting on method TestClass::stat()


isFinal():
bool(false)

isAbstract():
bool(false)

isPublic():
bool(true)

isPrivate():
bool(false)

isProtected():
bool(false)

isStatic():
bool(true)

isConstructor():
bool(false)

isDestructor():
bool(false)

**********************************
**********************************
Reflecting on method TestClass::priv()


isFinal():
bool(false)

isAbstract():
bool(false)

isPublic():
bool(false)

isPrivate():
bool(true)

isProtected():
bool(false)

isStatic():
bool(false)

isConstructor():
bool(false)

isDestructor():
bool(false)

**********************************
**********************************
Reflecting on method TestClass::prot()


isFinal():
bool(false)

isAbstract():
bool(false)

isPublic():
bool(false)

isPrivate():
bool(false)

isProtected():
bool(true)

isStatic():
bool(false)

isConstructor():
bool(false)

isDestructor():
bool(false)

**********************************
**********************************
Reflecting on method DerivedClass::prot()


isFinal():
bool(false)

isAbstract():
bool(false)

isPublic():
bool(false)

isPrivate():
bool(false)

isProtected():
bool(true)

isStatic():
bool(false)

isConstructor():
bool(false)

isDestructor():
bool(false)

**********************************
**********************************
Reflecting on method TestInterface::int()


isFinal():
bool(false)

isAbstract():
bool(true)

isPublic():
bool(true)

isPrivate():
bool(false)

isProtected():
bool(false)

isStatic():
bool(false)

isConstructor():
bool(false)

isDestructor():
bool(false)

**********************************
**********************************
Reflecting on method ReflectionProperty::__construct()


isFinal():
bool(false)

isAbstract():
bool(false)

isPublic():
bool(true)

isPrivate():
bool(false)

isProtected():
bool(false)

isStatic():
bool(false)

isConstructor():
bool(true)

isDestructor():
bool(false)

**********************************
**********************************
Reflecting on method TestClass::__destruct()


isFinal():
bool(false)

isAbstract():
bool(false)

isPublic():
bool(true)

isPrivate():
bool(false)

isProtected():
bool(false)

isStatic():
bool(false)

isConstructor():
bool(false)

isDestructor():
bool(true)

**********************************


