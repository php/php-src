--TEST--
Test password_needs_rehash() with bcrypt-sha256
--FILE--
<?php
//-=-=-=-

$h5 = password_hash("foo", PASSWORD_BCRYPT_SHA256, ["cost" => 5]);

// Same cost -> no rehash
var_dump(password_needs_rehash($h5, PASSWORD_BCRYPT_SHA256, ["cost" => 5]));
// Different cost -> rehash
var_dump(password_needs_rehash($h5, PASSWORD_BCRYPT_SHA256, ["cost" => 8]));
// Different algorithm -> rehash
var_dump(password_needs_rehash($h5, PASSWORD_BCRYPT));
// Unrecognized hash -> rehash
var_dump(password_needs_rehash("", PASSWORD_BCRYPT_SHA256));

echo "OK!";
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
OK!
