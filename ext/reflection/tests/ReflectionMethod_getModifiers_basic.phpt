--TEST--
ReflectionMethod::getModifiers()
--FILE--
<?php

function reflectMethodModifiers($class) {
    $classInfo = new reflectionClass($class);
    $methodArray = $classInfo->getMethods();

    foreach ($methodArray as $method) {
        echo "Modifiers for method $method->class::$method->name():\n";
        printf("0x%08x\n", $method->getModifiers());
        echo "\n\n";
    }
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

    public final function fin() {}

    public function __destruct() {}

    public function __call($a, $b) {}

    public function __clone() {}

    public function __get($a) {}

    public function __set($a, $b) {}

    public function __unset($a) {}

    public function __isset($a) {}

    public function __tostring() {}

    public function __sleep() {}

    public function __wakeup() {}

    public function __set_state() {}

    public function __autoload() {}
}

class DerivedClass extends TestClass {}

interface TestInterface {
	public function int();
	public function __clone();
}

abstract class AbstractClass {
	public abstract function foo();
}



reflectMethodModifiers("TestClass");
reflectMethodModifiers("DerivedClass");
reflectMethodModifiers("TestInterface");
reflectMethodModifiers("AbstractClass");

echo "Wrong number of params:\n";
$a = new ReflectionMethod('TestClass::foo');
$a->getModifiers(1);

$a = new ReflectionMethod('ReflectionMethod::getModifiers');

echo "\nReflectionMethod::getModifiers() modifiers:\n";
printf("0x%08x\n", $a->getModifiers());

?>
--EXPECTF--
Modifiers for method TestClass::foo():
0x00000001


Modifiers for method TestClass::stat():
0x00000011


Modifiers for method TestClass::priv():
0x00000004


Modifiers for method TestClass::prot():
0x00000002


Modifiers for method TestClass::fin():
0x00000021


Modifiers for method TestClass::__destruct():
0x00000001


Modifiers for method TestClass::__call():
0x00000001


Modifiers for method TestClass::__clone():
0x00000001


Modifiers for method TestClass::__get():
0x00000001


Modifiers for method TestClass::__set():
0x00000001


Modifiers for method TestClass::__unset():
0x00000001


Modifiers for method TestClass::__isset():
0x00000001


Modifiers for method TestClass::__tostring():
0x00000001


Modifiers for method TestClass::__sleep():
0x00000001


Modifiers for method TestClass::__wakeup():
0x00000001


Modifiers for method TestClass::__set_state():
0x00000001


Modifiers for method TestClass::__autoload():
0x00000001


Modifiers for method TestClass::foo():
0x00000001


Modifiers for method TestClass::stat():
0x00000011


Modifiers for method TestClass::priv():
0x00000004


Modifiers for method TestClass::prot():
0x00000002


Modifiers for method TestClass::fin():
0x00000021


Modifiers for method TestClass::__destruct():
0x00000001


Modifiers for method TestClass::__call():
0x00000001


Modifiers for method TestClass::__clone():
0x00000001


Modifiers for method TestClass::__get():
0x00000001


Modifiers for method TestClass::__set():
0x00000001


Modifiers for method TestClass::__unset():
0x00000001


Modifiers for method TestClass::__isset():
0x00000001


Modifiers for method TestClass::__tostring():
0x00000001


Modifiers for method TestClass::__sleep():
0x00000001


Modifiers for method TestClass::__wakeup():
0x00000001


Modifiers for method TestClass::__set_state():
0x00000001


Modifiers for method TestClass::__autoload():
0x00000001


Modifiers for method TestInterface::int():
0x00000041


Modifiers for method TestInterface::__clone():
0x00000041


Modifiers for method AbstractClass::foo():
0x00000041


Wrong number of params:

Warning: ReflectionMethod::getModifiers() expects exactly 0 parameters, 1 given in %s on line %d

ReflectionMethod::getModifiers() modifiers:
0x00000001
