--TEST--
Test error operation of password_hash()
--FILE--
<?php
//-=-=-=-

try {
    var_dump(password_hash("foo"));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    password_hash("foo", array());
} catch (TypeError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    var_dump(password_hash("foo", 19, new StdClass));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(password_hash("foo", PASSWORD_BCRYPT, "baz"));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump(password_hash(array(), PASSWORD_BCRYPT));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ArgumentCountError: password_hash() expects at least 2 arguments, 1 given
TypeError: password_hash(): Argument #2 ($algo) must be of type string|int|null, array given
TypeError: password_hash(): Argument #3 ($options) must be of type array, stdClass given
TypeError: password_hash(): Argument #3 ($options) must be of type array, string given
TypeError: password_hash(): Argument #1 ($password) must be of type string, array given
