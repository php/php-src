--TEST--
ReflectionClass::getStaticPropertyValue()
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

echo "Retrieving static values from A:\n";
$rcA = new ReflectionClass('A');
var_dump($rcA->getStaticPropertyValue("privateDoesNotExist", "default value"));
var_dump($rcA->getStaticPropertyValue("privateOverridden"));
var_dump($rcA->getStaticPropertyValue("protectedDoesNotExist", "default value"));
var_dump($rcA->getStaticPropertyValue("protectedOverridden"));
var_dump($rcA->getStaticPropertyValue("publicOverridden"));

echo "\nRetrieving static values from B:\n";
$rcB = new ReflectionClass('B');
var_dump($rcB->getStaticPropertyValue("privateOverridden"));
var_dump($rcB->getStaticPropertyValue("protectedOverridden"));
var_dump($rcB->getStaticPropertyValue("publicOverridden"));

echo "\nRetrieving non-existent values from A with no default value:\n";
try {
    var_dump($rcA->getStaticPropertyValue("protectedDoesNotExist"));
    echo "you should not see this";
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump($rcA->getStaticPropertyValue("privateDoesNotExist"));
    echo "you should not see this";
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Retrieving static values from A:
string(13) "default value"
string(16) "original private"
string(13) "default value"
string(18) "original protected"
string(15) "original public"

Retrieving static values from B:
string(15) "changed private"
string(17) "changed protected"
string(14) "changed public"

Retrieving non-existent values from A with no default value:
Property A::$protectedDoesNotExist does not exist
Property A::$privateDoesNotExist does not exist
