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
$rcA->setStaticPropertyValue("privateOverridden", "new value 1");
$rcA->setStaticPropertyValue("protectedOverridden", "new value 2");
$rcA->setStaticPropertyValue("publicOverridden", "new value 3");
print_r($rcA->getStaticProperties());

echo "\nSet static values in B:\n";
$rcB = new ReflectionClass('B');
$rcB->setStaticPropertyValue("privateOverridden", "new value 4");
$rcB->setStaticPropertyValue("privateOverridden", "new value 5");
$rcB->setStaticPropertyValue("protectedOverridden", "new value 6");
$rcB->setStaticPropertyValue("publicOverridden", "new value 7");
print_r($rcA->getStaticProperties());
print_r($rcB->getStaticProperties());

echo "\nSet non-existent values from A with no default value:\n";
try {
    var_dump($rcA->setStaticPropertyValue("protectedDoesNotExist", "new value 8"));
    echo "you should not see this";
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump($rcA->setStaticPropertyValue("privateDoesNotExist", "new value 9"));
    echo "you should not see this";
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Set static values in A:
Array
(
    [privateOverridden] => new value 1
    [protectedOverridden] => new value 2
    [publicOverridden] => new value 3
)

Set static values in B:
Array
(
    [privateOverridden] => new value 1
    [protectedOverridden] => new value 2
    [publicOverridden] => new value 3
)
Array
(
    [privateOverridden] => new value 5
    [protectedOverridden] => new value 6
    [publicOverridden] => new value 7
)

Set non-existent values from A with no default value:
Class A does not have a property named protectedDoesNotExist
Class A does not have a property named privateDoesNotExist
