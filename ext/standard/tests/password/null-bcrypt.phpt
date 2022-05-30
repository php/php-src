--TEST--
password_hash(bcrypt) should error on a null byte in the value
--FILE--
<?php

$apwd = password_hash("a", PASSWORD_BCRYPT);
var_dump($apwd);
var_dump(password_verify("a", $apwd));
var_dump(password_verify("a\0", $apwd));

try {
	$apwd = password_hash("a\0", PASSWORD_BCRYPT);
} catch (ValueError $ex) {
	echo 'Exception: ', $ex->getMessage(), "\n";
}

--EXPECTF--
string(60) "$2y$%s"
bool(true)

Warning: password_verify(): bcrypt %s regenerated %s argon2i/argon2id in %s/null-bcrypt.php on line %d
bool(true)
Exception: bcrypt based password hashing is not binary safe, but the provided password contains a null byte
