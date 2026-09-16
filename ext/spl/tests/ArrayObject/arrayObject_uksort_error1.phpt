--TEST--
ArrayObject::uksort() function: non callable error
--FILE--
<?php

$ao = new ArrayObject();

try {
    $ao->uksort('not_a_valid_function');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: uksort(): Argument #2 ($callback) must be a valid callback, function "not_a_valid_function" not found or invalid function name
