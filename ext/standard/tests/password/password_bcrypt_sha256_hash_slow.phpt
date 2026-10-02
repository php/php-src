--TEST--
Test normal operation of password_hash() with bcrypt-sha256
--SKIPIF--
<?php if (getenv("SKIP_SLOW_TESTS")) die("skip slow test"); ?>
--FILE--
<?php

$h = password_hash("foo", PASSWORD_BCRYPT_SHA256);
var_dump(preg_match('#^\$bcrypt-sha256\$v=2,t=2b,r=12\$[./A-Za-z0-9]{22}\$[./A-Za-z0-9]{31}$#', $h) === 1);

// Round-trip: correct password verifies, wrong one does not
var_dump(password_verify("foo", $h));
var_dump(password_verify("bar", $h));

echo "OK!";
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
OK!
