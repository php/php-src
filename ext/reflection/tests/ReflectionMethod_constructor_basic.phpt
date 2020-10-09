--TEST--
ReflectionMethod::isConstructor()
--FILE--
<?php

class NewCtor {
    function __construct() {
        echo "In " . __METHOD__ . "\n";
    }

}
echo "New-style constructor:\n";
$methodInfo = new ReflectionMethod("NewCtor::__construct");
var_dump($methodInfo->isConstructor());

class ExtendsNewCtor extends NewCtor {
}
echo "\nInherited new-style constructor\n";
$methodInfo = new ReflectionMethod("ExtendsNewCtor::__construct");
var_dump($methodInfo->isConstructor());

class X {
    function Y() {
        echo "In " . __METHOD__ . "\n";
    }
}
echo "\nNot a constructor:\n";
$methodInfo = new ReflectionMethod("X::Y");
var_dump($methodInfo->isConstructor());

class Y extends X {
}
echo "\nInherited method of the same name as the class:\n";
$methodInfo = new ReflectionMethod("Y::Y");
var_dump($methodInfo->isConstructor());

?>
--EXPECT--
New-style constructor:
bool(true)

Inherited new-style constructor
bool(true)

Not a constructor:
bool(false)

Inherited method of the same name as the class:
bool(false)
