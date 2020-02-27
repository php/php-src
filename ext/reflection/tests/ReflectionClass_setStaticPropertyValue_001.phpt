--TEST--
ReflectionClass::setStaticPropertyValue()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php
class A {
    static private $privateOverridden = "original private";
    static protected $protectedOverridden = "original protected";
    static public $publicOverridden = "original public";
}

class B extends A {
    static private $privateOverridden = "changed private";
    static protected $protectedOverridden = "changed protected";
    static public $publicOverridden = "changed public";
}

echo "Set static values in A:\n";
$rcA = new ReflectionClass('A');
$rcA->setStaticPropertyValue("\0A\0privateOverridden", "new value 1");
$rcA->setStaticPropertyValue("\0*\0protectedOverridden", "new value 2");
$rcA->setStaticPropertyValue("publicOverridden", "new value 3");
print_r($rcA->getStaticProperties());

echo "\nSet static values in B:\n";
$rcB = new ReflectionClass('B');
$rcB->setStaticPropertyValue("\0A\0privateOverridden", "new value 4");
$rcB->setStaticPropertyValue("\0B\0privateOverridden", "new value 5");
$rcB->setStaticPropertyValue("\0*\0protectedOverridden", "new value 6");
$rcB->setStaticPropertyValue("publicOverridden", "new value 7");
print_r($rcA->getStaticProperties());
print_r($rcB->getStaticProperties());

echo "\nSet non-existent values from A with no default value:\n";
try {
    var_dump($rcA->setStaticPropertyValue("protectedOverridden", "new value 8"));
    echo "you should not see this";
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump($rcA->setStaticPropertyValue("privateOverridden", "new value 9"));
    echo "you should not see this";
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Set static values in A:

Fatal error: Uncaught ReflectionException: Class A does not have a property named  in %s:%d
Stack trace:
#0 %s(%d): ReflectionClass->setStaticPropertyValue('\x00A\x00privateOverr...', 'new value 1')
#1 {main}
  thrown in %s on line %d
