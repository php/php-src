--TEST--
Test normal operation of password_hash() with bcrypt-sha256
--FILE--
<?php

$h4 = password_hash("foo", PASSWORD_BCRYPT_SHA256, ["cost" => 4]);
var_dump(preg_match('#^\$bcrypt-sha256\$v=2,t=2b,r=4\$[./A-Za-z0-9]{22}\$[./A-Za-z0-9]{31}$#', $h4) === 1);
var_dump(password_verify("foo", $h4));
var_dump(password_verify("bar", $h4));

// New algorithm is advertised by password_algos()
var_dump(in_array(PASSWORD_BCRYPT_SHA256, password_algos()));

echo "OK!";
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
OK!
