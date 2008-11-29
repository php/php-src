--TEST--
Test ReflectionProperty::setAccessible().
--FILE--
<?php
class A {
    protected $protected = 'a';
    protected static $protectedStatic = 'b';
    private $private = 'c';
    private static $privateStatic = 'd';
}

$a               = new A;
$protected       = new ReflectionProperty($a, 'protected');
$protectedStatic = new ReflectionProperty('A', 'protectedStatic');
$private         = new ReflectionProperty($a, 'private');
$privateStatic   = new ReflectionProperty('A', 'privateStatic');

try {
    var_dump($protected->getValue($a));
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    var_dump($protectedStatic->getValue());
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    var_dump($private->getValue($a));
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

try {
    var_dump($privateStatic->getValue());
}

catch (ReflectionException $e) {
    var_dump($e->getMessage());
}

$protected->setAccessible(TRUE);
$protectedStatic->setAccessible(TRUE);
$private->setAccessible(TRUE);
$privateStatic->setAccessible(TRUE);

var_dump($protected->getValue($a));
var_dump($protectedStatic->getValue());
var_dump($private->getValue($a));
var_dump($privateStatic->getValue());

$protected->setValue($a, 'e');
$protectedStatic->setValue('f');
$private->setValue($a, 'g');
$privateStatic->setValue('h');

var_dump($protected->getValue($a));
var_dump($protectedStatic->getValue());
var_dump($private->getValue($a));
var_dump($privateStatic->getValue());
?>
--EXPECT--
unicode(44) "Cannot access non-public member A::protected"
unicode(50) "Cannot access non-public member A::protectedStatic"
unicode(42) "Cannot access non-public member A::private"
unicode(48) "Cannot access non-public member A::privateStatic"
unicode(1) "a"
unicode(1) "b"
unicode(1) "c"
unicode(1) "d"
unicode(1) "e"
unicode(1) "f"
unicode(1) "g"
unicode(1) "h"
