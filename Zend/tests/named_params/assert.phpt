--TEST--
Calling assert with named params
--FILE--
<?php

assert(assertion: true);
try {
    assert(assertion: false);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

assert(assertion: true, description: "Description");
try {
    assert(assertion: false, description: "Description");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    assert(description: "Description");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: assert(assertion: false)
AssertionError: Description
Error: Named parameter $description overwrites previous argument
