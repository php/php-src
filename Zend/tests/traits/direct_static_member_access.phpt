--TEST--
Direct access to static trait members is deprecated
--FILE--
<?php

trait T {
    public static $foo;
    public static function foo() {
        echo "Foo\n";
    }
    public static function __callStatic($name, $args) {
        echo "CallStatic($name)\n";
    }
}

class C {
    use T;
}

function test() {
    T::$foo = 42;
    var_dump(T::$foo);
    T::foo();
    T::bar();
    echo "\n";
}

// Call twice to test cache slot behavior.
test();
test();

C::$foo = 42;
var_dump(C::$foo);
C::foo();
C::bar();

?>
--EXPECTF--
Deprecated: Accessing static trait property T::$foo is deprecated, it should only be accessed on a class using the trait in %s on line %d

Deprecated: Accessing static trait property T::$foo is deprecated, it should only be accessed on a class using the trait in %s on line %d
int(42)

Deprecated: Calling static trait method T::foo is deprecated, it should only be called on a class using the trait in %s on line %d
Foo

Deprecated: Calling static trait method T::bar is deprecated, it should only be called on a class using the trait in %s on line %d
CallStatic(bar)


Deprecated: Accessing static trait property T::$foo is deprecated, it should only be accessed on a class using the trait in %s on line %d

Deprecated: Accessing static trait property T::$foo is deprecated, it should only be accessed on a class using the trait in %s on line %d
int(42)

Deprecated: Calling static trait method T::foo is deprecated, it should only be called on a class using the trait in %s on line %d
Foo

Deprecated: Calling static trait method T::bar is deprecated, it should only be called on a class using the trait in %s on line %d
CallStatic(bar)

int(42)
Foo
CallStatic(bar)
