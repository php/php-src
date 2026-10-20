--TEST--
CallbackFilterIterator 002
--FILE--
<?php

set_error_handler(function($errno, $errstr){
    echo $errstr . "\n";
    return true;
});

try {
    new CallbackFilterIterator();
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new CallbackFilterIterator(null);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new CallbackFilterIterator(new ArrayIterator(array()), null);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new CallbackFilterIterator(new ArrayIterator(array()), array());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$it = new CallbackFilterIterator(new ArrayIterator(array(1)), function() {
    throw new Exception("some message");
});
try {
    foreach($it as $e);
} catch(Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ArgumentCountError: CallbackFilterIterator::__construct() expects exactly 2 arguments, 0 given
ArgumentCountError: CallbackFilterIterator::__construct() expects exactly 2 arguments, 1 given
TypeError: CallbackFilterIterator::__construct(): Argument #2 ($callback) must be a valid callback, no array or string given
TypeError: CallbackFilterIterator::__construct(): Argument #2 ($callback) must be a valid callback, array callback must have exactly two members
Exception: some message
