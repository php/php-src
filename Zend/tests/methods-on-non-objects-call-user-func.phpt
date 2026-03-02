--TEST--
call_user_func() in combination with "Call to a member function method() on a non-object"
--FILE--
<?php
$comparator = null;
try {
    var_dump(call_user_func([$comparator, 'compare'], 1, 2));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
call_user_func(): Argument #1 ($callback) must be a valid callback, first array member is not a valid class name or object
