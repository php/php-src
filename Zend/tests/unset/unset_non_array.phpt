--TEST--
Unset on non-array
--FILE--
<?php

unset($x[0]);

$x = null;
unset($x[0]);

$x = false;
unset($x[0]);

$x = true;
try {
    unset($x[0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$x = 1;
try {
    unset($x[0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$x = 3.14;
try {
    unset($x[0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$x = "str";
try {
    unset($x[0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$x = new stdClass;
try {
    unset($x[0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// And now repeat the same with a nested offset.
unset($x);

unset($x[0][0]);

$x = null;
unset($x[0][0]);

$x = false;
unset($x[0][0]);

$x = true;
try {
    unset($x[0][0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$x = 1;
try {
    unset($x[0][0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$x = 3.14;
try {
    unset($x[0][0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$x = "str";
try {
    unset($x[0][0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$x = new stdClass;
try {
    unset($x[0][0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: Undefined variable $x in %s on line %d

Deprecated: Automatic conversion of false to array is deprecated in %s
Cannot unset offset in a non-array variable
Cannot unset offset in a non-array variable
Cannot unset offset in a non-array variable
Cannot unset string offsets
Cannot use object of type stdClass as array

Warning: Undefined variable $x in %s on line %d

Deprecated: Automatic conversion of false to array is deprecated in %s
Cannot unset offset in a non-array variable
Cannot unset offset in a non-array variable
Cannot unset offset in a non-array variable
Cannot use string offset as an array
Cannot use object of type stdClass as array
