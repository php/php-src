--TEST--
Test bcrypt-sha256 normalizes unused padding bits like Passlib
--FILE--
<?php

// Canonical hash of "password" produced by Passlib 1.7.4.
$canonical = '$bcrypt-sha256$v=2,t=2b,r=5$5Hg1DKFqPE8C2aflZ5vVoe$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbS';

// The salt's last character is 'e'; 'f' has the same data bits but non-zero
// padding bits, so Passlib repairs it before using the salt as the HMAC key.
$dirty_salt = '$bcrypt-sha256$v=2,t=2b,r=5$5Hg1DKFqPE8C2aflZ5vVof$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbS';

// The digest's last character is 'S'; 'T' has the same data bits but non-zero
// padding bits, and is likewise repaired before comparison.
$dirty_digest = '$bcrypt-sha256$v=2,t=2b,r=5$5Hg1DKFqPE8C2aflZ5vVoe$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbT';

$dirty_both = '$bcrypt-sha256$v=2,t=2b,r=5$5Hg1DKFqPE8C2aflZ5vVof$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbT';

var_dump(password_verify("password", $canonical));
var_dump(password_verify("password", $dirty_salt));
var_dump(password_verify("password", $dirty_digest));
var_dump(password_verify("password", $dirty_both));
var_dump(password_verify("wrong", $dirty_both));

// Characters with different data bits are not equivalent: changing 'e' to
// another canonical salt character, or 'S' to another canonical digest
// character, must fail.
var_dump(password_verify("password", str_replace('Z5vVoe$', 'Z5vVou$', $canonical)));
var_dump(password_verify("password", str_replace('qyPbS', 'qyPbW', $canonical)));

echo "OK!\n";

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
OK!
