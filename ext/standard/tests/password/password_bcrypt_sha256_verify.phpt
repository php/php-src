--TEST--
Test password_verify() with bcrypt-sha256 (reference interop + edge cases)
--FILE--
<?php
//-=-=-=-

// Known-answer vector produced by passlib's bcrypt_sha256 (format version 2):
// password "password", salt "n79VH.0Q2TMWmt3Oqt9uku", cost 12.
// This pins the HMAC key/message order, the standard-base64 pre-hash encoding,
// the bcrypt computation and the hash-string parsing all at once.
$passlib = '$bcrypt-sha256$v=2,t=2b,r=12$n79VH.0Q2TMWmt3Oqt9uku$Kq4Noyk3094Y2QlB8NdRT8SvGiI4ft2';
var_dump(password_verify("password", $passlib));   // interop with passlib
var_dump(password_verify("wrong", $passlib));

// Long password (200 bytes, far over bcrypt's 72-byte limit) is fully considered.
$long = str_repeat("a", 200);
$hl = password_hash($long, PASSWORD_BCRYPT_SHA256);
var_dump(password_verify($long, $hl));             // true
var_dump(password_verify(str_repeat("a", 199), $hl)); // false (no 72-byte truncation)

// A NUL byte in the password is allowed (the pre-hash neutralizes the NUL quirk).
$nul = "foo\x00bar";
$hn = password_hash($nul, PASSWORD_BCRYPT_SHA256);
var_dump(password_verify($nul, $hn));             // true
var_dump(password_verify("foo\x00baz", $hn));     // false

// A malformed hash (invalid characters in the salt) is rejected.
var_dump(password_verify("password", '$bcrypt-sha256$v=2,t=2b,r=12$bad-salt!!$Kq4Noyk3094Y2QlB8NdRT8SvGiI4ft2'));

echo "OK!";
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
OK!
