--TEST--
Only arrays and Traversables can be unpacked
--FILE--
<?php

function test(...$args) {
    var_dump($args);
}

try {
    test(...null);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test(...42);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test(...new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    test(1, 2, 3, ..."foo", ...[4, 5]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test(1, 2, 3, ...new StdClass, ...3.14, ...[4, 5]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Only arrays and Traversables can be unpacked
Only arrays and Traversables can be unpacked
Only arrays and Traversables can be unpacked
Only arrays and Traversables can be unpacked
Only arrays and Traversables can be unpacked
