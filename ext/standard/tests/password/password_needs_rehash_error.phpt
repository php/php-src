--TEST--
Test error operation of password_needs_rehash()
--FILE--
<?php
//-=-=-=-

try {
    var_dump(password_needs_rehash(''));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(password_needs_rehash('', []));

try {
    var_dump(password_needs_rehash(array(), PASSWORD_BCRYPT));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(password_needs_rehash("", PASSWORD_BCRYPT, "foo"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "OK!";
?>
--EXPECT--
password_needs_rehash() expects at least 2 parameters, 1 given
bool(false)
password_needs_rehash(): Argument #1 ($hash) must be of type string, array given
password_needs_rehash(): Argument #3 ($options) must be of type array, string given
OK!
