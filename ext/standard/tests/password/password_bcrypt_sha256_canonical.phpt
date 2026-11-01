--TEST--
Test that bcrypt-sha256 generates canonical salts and digests
--FILE--
<?php

$salt_chars = ".Oeu";
$digest_chars = ".CGKOSWaeimquy26";
$ok = true;

for ($i = 0; $i < 64; $i++) {
    $hash = password_hash("password", PASSWORD_BCRYPT_SHA256, ["cost" => 4]);
    $parts = explode('$', $hash);
    $salt = $parts[3];
    $digest = $parts[4];

    if (strlen($salt) !== 22 || strpos($salt_chars, $salt[21]) === false) {
        echo "Non-canonical salt: $hash\n";
        $ok = false;
    }
    if (strlen($digest) !== 31 || strpos($digest_chars, $digest[30]) === false) {
        echo "Non-canonical digest: $hash\n";
        $ok = false;
    }
    if (!password_verify("password", $hash)) {
        echo "Failed to verify: $hash\n";
        $ok = false;
    }
}

var_dump($ok);
echo "OK!\n";

?>
--EXPECT--
bool(true)
OK!
