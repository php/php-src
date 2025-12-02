--TEST--
Basic type alias functionality
--FILE--
<?php
use type int|float as Number;
use type string as Text;
use type ?int as OptionalInt;
use type Number|string as Scalar;

function add(Number $a, Number $b): Number {
    return $a + $b;
}

function greet(Text $name): Text {
    return "Hello, " . $name;
}

function maybeInt(OptionalInt $value): OptionalInt {
    return $value;
}

function identity(Scalar $v): Scalar {
    return $v;
}

// Test int|float alias
var_dump(add(1, 2));
var_dump(add(1.5, 2.5));
var_dump(add(1, 2.5));

// Test string alias
var_dump(greet("World"));

// Test nullable alias
var_dump(maybeInt(42));
var_dump(maybeInt(null));

// Test nested alias (Scalar = Number|string = int|float|string)
var_dump(identity(42));
var_dump(identity(3.14));
var_dump(identity("hello"));

echo "Done\n";
?>
--EXPECT--
int(3)
float(4)
float(3.5)
string(12) "Hello, World"
int(42)
NULL
int(42)
float(3.14)
string(5) "hello"
Done
