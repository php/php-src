--TEST--
Freeing of function "name" when dynamic call fails
--FILE--
<?php

try {
    $bar = "bar";
    ("foo" . $bar)();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $bar = ["bar"];
    (["foo"] + $bar)();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    (new stdClass)();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Call to undefined function foobar()
Error: Array callback must have exactly two elements
Error: Object of type stdClass is not callable
