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
--EXPECTF--
password_needs_rehash() expects at least 2 parameters, 1 given
bool(false)
password_needs_rehash() expects parameter 1 to be string, array given
password_needs_rehash() expects parameter 3 to be array, string given
OK!
