--TEST--
call_user_func with wrong case fails
--FILE--
<?php
try {
    $result = call_user_func("STRLEN", "hello");
    echo $result . "\n";
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

$result2 = call_user_func("strlen", "hello");
echo $result2 . "\n";
?>
--EXPECTF--
call_user_func(): Argument #1 ($callback) must be a valid callback, function "STRLEN" not found or invalid function name
5
