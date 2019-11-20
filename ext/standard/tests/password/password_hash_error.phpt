--TEST--
Test error operation of password_hash()
--FILE--
<?php
//-=-=-=-

try {
    var_dump(password_hash("foo"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    password_hash("foo", array());
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump(password_hash("foo", 19, new StdClass));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(password_hash("foo", PASSWORD_BCRYPT, "baz"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(password_hash(array(), PASSWORD_BCRYPT));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
password_hash() expects at least 2 parameters, 1 given

Warning: Array to string conversion in %s on line %d
Unknown password hashing algorithm: Array
password_hash() expects parameter 3 to be array, object given
password_hash() expects parameter 3 to be array, string given
password_hash() expects parameter 1 to be string, array given
