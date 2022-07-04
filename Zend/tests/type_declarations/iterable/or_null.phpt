--TEST--
Test "or null"/"or be null" in type-checking errors for userland functions with iterable
--FILE--
<?php

// This should test every branch in zend_execute.c's `zend_verify_arg_type`, `zend_verify_return_type` and `zend_verify_missing_return_type` functions which produces an "or null"/"or be null" part in its error message

function iterableF(?iterable $param) {}
try {
    iterableF(1);
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnIterable(): ?iterable {
    return 1;
}

try {
    returnIterable();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}

function returnMissingIterable(): ?iterable {
}

try {
    returnMissingIterable();
} catch (\TypeError $e) {
    echo $e, PHP_EOL;
}
?>
--EXPECTF--
TypeError: iterableF(): Argument #1 ($param) must be of type Traversable|array|null, int given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): iterableF(1)
#1 {main}
TypeError: returnIterable(): Return value must be of type Traversable|array|null, int returned in %s:%d
Stack trace:
#0 %s(%d): returnIterable()
#1 {main}
TypeError: returnMissingIterable(): Return value must be of type Traversable|array|null, none returned in %s:%d
Stack trace:
#0 %s(%d): returnMissingIterable()
#1 {main}
