--TEST--
Test of password_hash() with bcrypt hashing for CVE-2024-3096
--FILE--
<?php
//-=-=-=-
try {
    $hash = password_hash("\0password", PASSWORD_BCRYPT);
    var_dump(password_verify('', $hash));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$hash = password_hash("p\0assword", PASSWORD_BCRYPT);
var_dump(password_verify('', $hash));
?>
--EXPECT--
Bcrypt password must not start with null character
bool(false)
