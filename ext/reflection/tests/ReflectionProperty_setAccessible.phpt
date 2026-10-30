--TEST--
Test that ReflectionProperty::setAccessible() has no effects
--FILE--
<?php
class A {
    protected $protected = 'a';
    protected static $protectedStatic = 'b';
    private $private = 'c';
    private static $privateStatic = 'd';
}

class B extends A {}

$a               = new A;
$protected       = new ReflectionProperty($a, 'protected');
$protectedStatic = new ReflectionProperty('A', 'protectedStatic');
$private         = new ReflectionProperty($a, 'private');
$privateStatic   = new ReflectionProperty('A', 'privateStatic');

var_dump($protected->getValue($a));
var_dump($protectedStatic->getValue());
var_dump($private->getValue($a));
var_dump($privateStatic->getValue());

$protected->setValue($a, 'e');
$protectedStatic->setValue(null, 'f');
$private->setValue($a, 'g');
$privateStatic->setValue(null, 'h');

var_dump($protected->getValue($a));
var_dump($protectedStatic->getValue());
var_dump($private->getValue($a));
var_dump($privateStatic->getValue());

$protected->setAccessible(FALSE);
$protectedStatic->setAccessible(FALSE);
$private->setAccessible(FALSE);
$privateStatic->setAccessible(FALSE);

var_dump($protected->getValue($a));
var_dump($protectedStatic->getValue());
var_dump($private->getValue($a));
var_dump($privateStatic->getValue());

$protected->setValue($a, 'i');
$protectedStatic->setValue('j');
$private->setValue($a, 'k');
$privateStatic->setValue(null, 'l');

var_dump($protected->getValue($a));
var_dump($protectedStatic->getValue());
var_dump($private->getValue($a));
var_dump($privateStatic->getValue());

$a               = new A;
$b               = new B;
$protected       = new ReflectionProperty($b, 'protected');
$protectedStatic = new ReflectionProperty('B', 'protectedStatic');
$private         = new ReflectionProperty($a, 'private');

var_dump($protected->getValue($b));
var_dump($protectedStatic->getValue());
var_dump($private->getValue($b));

$protected->setValue($b, 'e');
$protectedStatic->setValue(null, 'f');
$private->setValue($b, 'g');

var_dump($protected->getValue($b));
var_dump($protectedStatic->getValue());
var_dump($private->getValue($b));

$protected->setAccessible(FALSE);
$protectedStatic->setAccessible(FALSE);
$private->setAccessible(FALSE);

var_dump($protected->getValue($b));
var_dump($protectedStatic->getValue());
var_dump($private->getValue($b));

$protected->setValue($b, 'h');
$protectedStatic->setValue(null, 'i');
$private->setValue($b, 'j');

var_dump($protected->getValue($b));
var_dump($protectedStatic->getValue());
var_dump($private->getValue($b));
?>
--EXPECTF--
string(1) "a"
string(1) "b"
string(1) "c"
string(1) "d"
string(1) "e"
string(1) "f"
string(1) "g"
string(1) "h"
string(1) "e"
string(1) "f"
string(1) "g"
string(1) "h"

Deprecated: Calling ReflectionProperty::setValue() with a single argument is deprecated in %s on line %d
string(1) "i"
string(1) "j"
string(1) "k"
string(1) "l"
string(1) "a"
string(1) "j"
string(1) "c"
string(1) "e"
string(1) "f"
string(1) "g"
string(1) "e"
string(1) "f"
string(1) "g"
string(1) "h"
string(1) "i"
string(1) "j"
