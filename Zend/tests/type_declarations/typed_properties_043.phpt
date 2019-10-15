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
Cannot access typed static property Test::$selfProp on a trait, it may only be used as part of a class
Cannot access typed static property Test::$selfNullProp on a trait, it may only be used as part of a class
Cannot access typed static property Test::$parentProp on a trait, it may only be used as part of a class
object(Bar)#3 (0) {
}
object(Bar)#2 (0) {
}
object(Foo)#4 (0) {
}
