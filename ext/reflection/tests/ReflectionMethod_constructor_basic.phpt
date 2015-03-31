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


?>
--EXPECTF--
New-style constructor:
bool(true)

Inherited new-style constructor
bool(true)

