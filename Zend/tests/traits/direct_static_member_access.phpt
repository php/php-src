--TEST--
Cannot directly access static members on a trait
--FILE--
<?php

trait T {
    public static $foo;
    public static function foo() {
        echo "Foo\n";
    }
}

class C {
    use T;
}

try {
    T::$foo = 42;
    var_dump(T::$foo);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    T::foo();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

C::$foo = 42;
var_dump(C::$foo);

C::foo();

?>
--EXPECT--
Cannot access static property T::$foo on a trait, it may only be used as part of a class
Cannot call static method T::foo() on a trait, it may only be used as part of a class
int(42)
Foo
