--TEST--
GH-8623: Error message when creating first-class callable from undefined function
--FILE--
<?php

namespace Foo;

try {
    $f = 'missing'(...);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $f = missing(...);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $f = bar\missing(...);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
$name = 'missing';
try {
    $f = $name(...);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

/* The error message for actual calls is unchanged. */
try {
    missing();
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $name();
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

/* Partial applications are not affected for now. */
try {
    $f = missing(?);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Failed to create closure from undefined function missing()
Failed to create closure from undefined function Foo\missing()
Failed to create closure from undefined function Foo\bar\missing()
Failed to create closure from undefined function missing()
Call to undefined function Foo\missing()
Call to undefined function missing()
Call to undefined function Foo\missing()
