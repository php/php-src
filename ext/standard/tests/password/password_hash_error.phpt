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

var_dump(password_hash("foo", array()));

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

Warning: password_hash(): Unknown password hashing algorithm: Array in %s on line %d
NULL
password_hash() expects parameter 3 to be array, object given
password_hash() expects parameter 3 to be array, string given
password_hash() expects parameter 1 to be string, array given
