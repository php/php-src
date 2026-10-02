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

echo "OK!";
?>
--EXPECT--
ValueError: Invalid bcrypt cost parameter specified: 3
ValueError: Invalid bcrypt cost parameter specified: 32
OK!
