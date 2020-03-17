--TEST--
Test that final static properties can't be mutated by combined assignment operators
--FILE--
<?php

class Foo
{
    final public static int $property1 = 1;
    final public static ?string $property2;
}

$foo = new Foo();

try {
    Foo::$property1 += 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property1 -= 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property1 *= 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property1 /= 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property1 **= 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property1 &= 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property1 |= 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property1 ^= 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property1 <<= 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property1 >>= 1;
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

// $property2 must be initialized to null, otherwise the subsequent ??= would do nothing
Foo::$property2 ??= null;

try {
    Foo::$property2 ??= "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    Foo::$property2 .= "foo";
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(Foo::$property1);
var_dump(Foo::$property2);

?>
--EXPECT--
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property1 after initialization
Cannot modify final static property Foo::$property2 after initialization
Cannot modify final static property Foo::$property2 after initialization
int(1)
NULL
