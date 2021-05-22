--TEST--
Test is_initialized function
--FILE--
<?php
class Foo {
    public int $bar;
    public static int $qux;
    public $baz;

    private static int $boo = 10;

    public function is_private_static_initialized() {
        return is_initialized(self::class, "boo");
    }
}

$foo = new Foo();

try {
    is_initialized(42, "42"); # string|object
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

try {
    is_initialized(Unknown::class, "unknown"); # unknown class
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

try {
    is_initialized(Foo::class, "bar"); # instance property without object
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

try {
    is_initialized($foo, "baz"); # accessible but untyped
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

try {
    is_initialized($foo, "unknown"); # unknown
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

try {
    is_initialized($foo, "boo"); # unaccessible
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

$foo->dynamic = 42;

try {
    is_initialized($foo, "dynamic"); # dynamic
} catch (Error $ex) {
    printf("%s\n", $ex->getMessage());
}

var_dump(is_initialized($foo, "bar"),         # uninitialized
         is_initialized(Foo::class, "qux"));  # uninitialized
         
$foo->bar = 42;
Foo::$qux = 42;

var_dump(is_initialized($foo, "bar"),         # initialized
         is_initialized(Foo::class, "qux"));  # initialized

unset($foo->bar);

var_dump(is_initialized($foo, "bar"));             # uninitialized

var_dump($foo->is_private_static_initialized());   # initialized
?>
--EXPECT--
is_initialized(): Argument #1 ($scope) must be an object or string
is_initialized(): Argument #1 ($scope) must be the name of a loaded class
is_initialized(): Argument #1 ($scope) must be an object for instance property
is_initialized(): Argument #2 ($property) must be the name of an accessible typed property
is_initialized(): Argument #2 ($property) must be the name of an accessible typed property
is_initialized(): Argument #2 ($property) must be the name of an accessible typed property
is_initialized(): Argument #2 ($property) must be the name of an accessible typed property
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
