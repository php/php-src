--TEST--
Test password_verify() with bcrypt-sha256 (reference interop + edge cases)
--FILE--
<?php

// All good hashes of "password"
$hashes = [
    '$bcrypt-sha256$v=2,t=2b,r=4$eMxX19v0bSLLEMYlqptrpO$uGMD6RGp0uLkSsD9OGWaCJfzo0k/nU6',
    '$bcrypt-sha256$v=2,t=2b,r=5$mBLkw4xnF1.CJNbGzw5PN.$fHtytl6T2qbDdy.Rn82DNY.msOZkbd6',
    '$bcrypt-sha256$v=2,t=2b,r=6$QBFpAxG4Yq3WIQ40ucchL.$1qcUsEjc3wRevgO4609qn.PkiCdj8iK',
    '$bcrypt-sha256$v=2,t=2b,r=7$780gqyokqg1y4iDn1qO1xu$Nvzt8YvkOfRNv8HZWiKkkYWF6yJEfFC',
    '$bcrypt-sha256$v=2,t=2b,r=8$0XXZcHZZkC55wMAcaiKBle$whIS0hvUo5p/xkW3eE2Tv8A1vuLm9Gq',
];

foreach ($hashes as $hash) {
    var_dump(password_verify("password", $hash));
    var_dump(password_verify("wrong", $hash));
}

// Long password (200 bytes, far over bcrypt's 72-byte limit) is fully considered.
$long = str_repeat("a", 200);
$hl = password_hash($long, PASSWORD_BCRYPT_SHA256, ['cost' => 5]);
var_dump(password_verify($long, $hl));             // true
var_dump(password_verify(str_repeat("a", 199), $hl)); // false (no 72-byte truncation)

// A NUL byte in the password is allowed (the pre-hash neutralizes the NUL quirk).
$nul = "foo\x00bar";
$hn = password_hash($nul, PASSWORD_BCRYPT_SHA256, ['cost' => 5]);
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
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
OK!
