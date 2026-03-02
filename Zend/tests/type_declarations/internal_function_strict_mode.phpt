--TEST--
Scalar type - internal function strict mode
--FILE--
<?php
declare(strict_types=1);

echo "*** Trying Ord With Integer" . PHP_EOL;
try {
    var_dump(ord(1));
} catch (TypeError $e) {
    echo "*** Caught " . $e->getMessage() . PHP_EOL;
}

echo "*** Trying Array Map With Invalid Callback" . PHP_EOL;
try {
    array_map([null, "bar"], []);
} catch (TypeError $e) {
    echo "*** Caught " . $e->getMessage() . PHP_EOL;
}

echo "*** Trying Strlen With Float" . PHP_EOL;
try {
    var_dump(strlen(1.5));
} catch (TypeError $e) {
    echo "*** Caught " . $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
*** Trying Ord With Integer
*** Caught ord(): Argument #1 ($character) must be of type string, int given
*** Trying Array Map With Invalid Callback
*** Caught array_map(): Argument #1 ($callback) must be a valid callback or null, first array member is not a valid class name or object
*** Trying Strlen With Float
*** Caught strlen(): Argument #1 ($string) must be of type string, float given
