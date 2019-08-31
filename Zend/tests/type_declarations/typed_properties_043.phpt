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
--EXPECT--
Cannot write a value to a 'self' typed static property of a trait
Cannot write a non-null value to a 'self' typed static property of a trait
Cannot access parent:: when current class scope has no parent
NULL
object(Bar)#3 (0) {
}
object(Bar)#2 (0) {
}
object(Foo)#4 (0) {
}
