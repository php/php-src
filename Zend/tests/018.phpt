--TEST--
constant() tests
--FILE--
<?php


define("TEST_CONST", 1);
try {
    var_dump(constant(""));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    constant('::');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

define("TEST_CONST2", "test");
var_dump(constant("TEST_CONST2"));

echo "Done\n";
?>
--EXPECTF--
Couldn't find constant 
Couldn't find constant ::
string(4) "test"
Done
