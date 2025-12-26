--TEST--
include types functionality
--FILE--
<?php
include types 'types.php';

function add(Number $a, Number $b): Number {
    return $a + $b;
}

function identity(Scalar $v): Scalar {
    return $v;
}

function greet(OptionalString $name): OptionalString {
    return $name ? "Hello, " . $name : null;
}

// Test Number alias (int|float)
var_dump(add(1, 2));
var_dump(add(1.5, 2.5));

// Test Scalar alias (Number|string = int|float|string)
var_dump(identity(42));
var_dump(identity(3.14));
var_dump(identity("hello"));

// Test OptionalString alias (?string)
var_dump(greet("World"));
var_dump(greet(null));

echo "Done\n";
?>
--EXPECT--
int(3)
float(4)
int(42)
float(3.14)
string(5) "hello"
string(12) "Hello, World"
NULL
Done
