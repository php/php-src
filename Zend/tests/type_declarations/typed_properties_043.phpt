--TEST--
Trying to assign to a static 'self' typed property on a trait must not fixate the type to the trait
--FILE--
<?php

trait Test {
    public static self $selfProp;
    public static ?self $selfNullProp;
    public static parent $parentProp;
}

try {
    Test::$selfProp = new stdClass;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    Test::$selfNullProp = new stdClass;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    Test::$parentProp = new stdClass;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

Test::$selfNullProp = null;
var_dump(Test::$selfNullProp);

class Foo {}
class Bar extends Foo {
    use Test;
}

Bar::$selfProp = new Bar;
Bar::$selfNullProp = new Bar;
Bar::$parentProp = new Foo;

var_dump(Bar::$selfProp, Bar::$selfNullProp, Bar::$parentProp);

?>
--EXPECTF--
Deprecated: Accessing static trait property Test::$selfProp is deprecated, it should only be accessed on a class using the trait in %s on line %d
Cannot assign stdClass to property Test::$selfProp of type self

Deprecated: Accessing static trait property Test::$selfNullProp is deprecated, it should only be accessed on a class using the trait in %s on line %d
Cannot assign stdClass to property Test::$selfNullProp of type ?self

Deprecated: Accessing static trait property Test::$parentProp is deprecated, it should only be accessed on a class using the trait in %s on line %d
Cannot assign stdClass to property Test::$parentProp of type parent

Deprecated: Accessing static trait property Test::$selfNullProp is deprecated, it should only be accessed on a class using the trait in %s on line %d

Deprecated: Accessing static trait property Test::$selfNullProp is deprecated, it should only be accessed on a class using the trait in %s on line %d
NULL
object(Bar)#3 (0) {
}
object(Bar)#2 (0) {
}
object(Foo)#4 (0) {
}
