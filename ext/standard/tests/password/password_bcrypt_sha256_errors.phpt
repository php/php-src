--TEST--
Test error operation of password_hash() with bcrypt-sha256
--FILE--
<?php
//-=-=-=-
try {
    password_hash("foo", PASSWORD_BCRYPT_SHA256, ["cost" => 3]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    password_hash("foo", PASSWORD_BCRYPT_SHA256, ["cost" => 32]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Unlike bcrypt, a NUL byte is allowed: the HMAC pre-hash removes the quirk.
var_dump(password_verify("foo\x00bar", password_hash("foo\x00bar", PASSWORD_BCRYPT_SHA256)));

echo "OK!";
?>
--EXPECT--
ValueError: Invalid bcrypt cost parameter specified: 3
ValueError: Invalid bcrypt cost parameter specified: 32
bool(true)
OK!
