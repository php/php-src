--TEST--
Calling assert with named params
--FILE--
<?php

assert(assertion: true);
try {
    assert(assertion: false);
} catch (AssertionError $e) {
    echo $e->getMessage(), "\n";
}

assert(assertion: true, description: "Description");
try {
    assert(assertion: false, description: "Description");
} catch (AssertionError $e) {
    echo $e->getMessage(), "\n";
}

try {
    assert(description: "Description");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(assertion: false)
Description
Named parameter $description overwrites previous argument
