--TEST--
Test normal operation of password_hash() with bcrypt-sha256
--FILE--
<?php
//-=-=-=-

// Format check (default cost 12 -> 2 digit rounds, 22-char salt, 31-char digest)
$h = password_hash("foo", PASSWORD_BCRYPT_SHA256);
var_dump(preg_match('#^\$bcrypt-sha256\$v=2,t=2b,r=12\$[./A-Za-z0-9]{22}\$[./A-Za-z0-9]{31}$#', $h) === 1);

// Round-trip: correct password verifies, wrong one does not
var_dump(password_verify("foo", $h));
var_dump(password_verify("bar", $h));

// 1-digit cost
$h4 = password_hash("foo", PASSWORD_BCRYPT_SHA256, ["cost" => 4]);
var_dump(preg_match('#^\$bcrypt-sha256\$v=2,t=2b,r=4\$[./A-Za-z0-9]{22}\$[./A-Za-z0-9]{31}$#', $h4) === 1);
var_dump(password_verify("foo", $h4));

// The string ident works too
$h2 = password_hash("foo", "bcrypt-sha256");
var_dump(password_verify("foo", $h2));

// New algorithm is advertised by password_algos()
var_dump(in_array("bcrypt-sha256", password_algos()));

echo "OK!";
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
OK!
