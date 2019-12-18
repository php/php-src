--TEST--
Freeing of function "name" when dynamic call fails
--FILE--
<?php

try {
    $bar = "bar";
    ("foo" . $bar)();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $bar = ["bar"];
    (["foo"] + $bar)();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    (new stdClass)();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to undefined function foobar()
Function name must be a string
Function name must be a string
