--TEST--
Test array_map() function : usage variations - failing built-in functions & language constructs
--FILE--
<?php

$arg = [1, 2];

// built-in functions & language constructs
$callbacks = [
    'echo',
    'array',
    'empty',
    'eval',
    'isset',
    'list',
    'print',
];

foreach($callbacks as $callback)
{
    try {
        var_dump( array_map($callback, $arg) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "Done";
?>
--EXPECT--
array_map(): Argument #1 ($callback) must be a valid callback or null, function "echo" not found or invalid function name
array_map(): Argument #1 ($callback) must be a valid callback or null, function "array" not found or invalid function name
array_map(): Argument #1 ($callback) must be a valid callback or null, function "empty" not found or invalid function name
array_map(): Argument #1 ($callback) must be a valid callback or null, function "eval" not found or invalid function name
array_map(): Argument #1 ($callback) must be a valid callback or null, function "isset" not found or invalid function name
array_map(): Argument #1 ($callback) must be a valid callback or null, function "list" not found or invalid function name
array_map(): Argument #1 ($callback) must be a valid callback or null, function "print" not found or invalid function name
Done
