--TEST--
constant() tests
--FILE--
<?php

try {
    var_dump(constant(""));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

define("TEST_CONST", 1);
var_dump(constant("TEST_CONST"));

define("TEST_CONST2", "test");
var_dump(constant("TEST_CONST2"));

echo "Done\n";
?>
--EXPECT--
Error: Undefined constant ""
int(1)
string(4) "test"
Done
