--TEST--
ReflectionMethod::getModifiers()
--FILE--
<?php

function reflectMethodModifiers($class) {
    $classInfo = new reflectionClass($class);
    $methodArray = $classInfo->getMethods();

    foreach ($methodArray as $method) {
        echo "Modifiers for method $method->class::$method->name():\n";
        var_dump($method->getModifiers());
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
var_dump($a->getModifiers());

?>
--EXPECTF--
Modifiers for method TestClass::foo():
int(65792)


Modifiers for method TestClass::stat():
int(257)


Modifiers for method TestClass::priv():
int(66560)


Modifiers for method TestClass::prot():
int(66048)


Modifiers for method TestClass::fin():
int(65796)


Modifiers for method TestClass::__destruct():
int(16640)


Modifiers for method TestClass::__call():
int(256)


Modifiers for method TestClass::__clone():
int(33024)


Modifiers for method TestClass::__get():
int(256)


Modifiers for method TestClass::__set():
int(256)


Modifiers for method TestClass::__unset():
int(256)


Modifiers for method TestClass::__isset():
int(256)


Modifiers for method TestClass::__tostring():
int(256)


Modifiers for method TestClass::__sleep():
int(65792)


Modifiers for method TestClass::__wakeup():
int(65792)


Modifiers for method TestClass::__set_state():
int(65792)


Modifiers for method TestClass::__autoload():
int(65792)


Modifiers for method TestClass::foo():
int(65792)


Modifiers for method TestClass::stat():
int(257)


Modifiers for method TestClass::priv():
int(66560)


Modifiers for method TestClass::prot():
int(66048)


Modifiers for method TestClass::fin():
int(65796)


Modifiers for method TestClass::__destruct():
int(16640)


Modifiers for method TestClass::__call():
int(256)


Modifiers for method TestClass::__clone():
int(33024)


Modifiers for method TestClass::__get():
int(256)


Modifiers for method TestClass::__set():
int(256)


Modifiers for method TestClass::__unset():
int(256)


Modifiers for method TestClass::__isset():
int(256)


Modifiers for method TestClass::__tostring():
int(256)


Modifiers for method TestClass::__sleep():
int(65792)


Modifiers for method TestClass::__wakeup():
int(65792)


Modifiers for method TestClass::__set_state():
int(65792)


Modifiers for method TestClass::__autoload():
int(65792)


Modifiers for method TestInterface::int():
int(258)


Modifiers for method TestInterface::__clone():
int(258)


Modifiers for method AbstractClass::foo():
int(65794)


Wrong number of params:

Warning: ReflectionMethod::getModifiers() expects exactly 0 parameters, 1 given in %s on line %d

ReflectionMethod::getModifiers() modifiers:
int(256)
