--TEST--
ReflectionClass::getConstant()
--FILE--
<?php
class C {
    const a = 'hello from C';
}
class D extends C {
}
class E extends D {
}
class F extends E {
    const a = 'hello from F';
}
class X {
}

$classes = array("C", "D", "E", "F", "X");
foreach($classes as $class) {
    echo "Reflecting on class $class:\n";
    $rc = new ReflectionClass($class);
    var_dump($rc->getConstant('a'));
    var_dump($rc->getConstant('doesnotexist'));
}
?>
--EXPECTF--
Reflecting on class C:
string(12) "hello from C"

Deprecated: ReflectionClass::getConstant() for a non-existent constant is deprecated, use ReflectionClass::hasConstant() to check if the constant exists in %s on line %d
bool(false)
Reflecting on class D:
string(12) "hello from C"

Deprecated: ReflectionClass::getConstant() for a non-existent constant is deprecated, use ReflectionClass::hasConstant() to check if the constant exists in %s on line %d
bool(false)
Reflecting on class E:
string(12) "hello from C"

Deprecated: ReflectionClass::getConstant() for a non-existent constant is deprecated, use ReflectionClass::hasConstant() to check if the constant exists in %s on line %d
bool(false)
Reflecting on class F:
string(12) "hello from F"

Deprecated: ReflectionClass::getConstant() for a non-existent constant is deprecated, use ReflectionClass::hasConstant() to check if the constant exists in %s on line %d
bool(false)
Reflecting on class X:

Deprecated: ReflectionClass::getConstant() for a non-existent constant is deprecated, use ReflectionClass::hasConstant() to check if the constant exists in %s on line %d
bool(false)

Deprecated: ReflectionClass::getConstant() for a non-existent constant is deprecated, use ReflectionClass::hasConstant() to check if the constant exists in %s on line %d
bool(false)
