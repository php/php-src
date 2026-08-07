--TEST--
Unknown named parameter
--FILE--
<?php

function test($a) {
}

function test2(...$a) {
}

try {
    test(b: 42);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    test(b: new stdClass);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    test(b: 2, a: 1);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    test(...new ArrayIterator(['unknown' => 42]));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    test2(a: 42);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Unknown named parameter $b
Error: Unknown named parameter $b
Error: Unknown named parameter $b
Error: Unknown named parameter $unknown
