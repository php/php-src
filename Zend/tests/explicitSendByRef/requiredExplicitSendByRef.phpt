--TEST--
require_explicit_send_by_ref declare
--FILE--
<?php

declare(require_explicit_send_by_ref=1);

function test1($foo, &$bar) {
    var_dump($foo);
    var_dump($bar++);
}

function id($arg) {
    return $arg;
}

$a = "foo";
$a .= "bar";
$b = "bar";
$b .= "foo";
$ref = 0;

// Early bound function
try {
    test1($a, $b);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test1($a, $b."baz");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test1($a, "foobar");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test1($a, id($b));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    test1($a, &$ref);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test1($a."baz", &$ref);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test1("barfoo", &$ref);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test1(id($a), &$ref);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// Late-bound function
try {
    test2($a, $b);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2($a, $b."baz");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2($a, "foobar");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2($a, id($b));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$ref = 0;
try {
    test2($a, &$ref);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2($a."baz", &$ref);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2("barfoo", &$ref);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2(id($a), &$ref);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

function test2($foo, &$bar) {
    var_dump($foo);
    var_dump($bar++);
}

?>
--EXPECT--
Cannot pass parameter 2 by reference
Cannot pass parameter 2 by reference
Cannot pass parameter 2 by reference
Cannot pass parameter 2 by reference
string(6) "foobar"
int(0)
string(9) "foobarbaz"
int(1)
string(6) "barfoo"
int(2)
string(6) "foobar"
int(3)
Cannot pass parameter 2 by reference
Cannot pass parameter 2 by reference
Cannot pass parameter 2 by reference
Cannot pass parameter 2 by reference
string(6) "foobar"
int(0)
string(9) "foobarbaz"
int(1)
string(6) "barfoo"
int(2)
string(6) "foobar"
int(3)
