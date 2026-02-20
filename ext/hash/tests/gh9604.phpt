--TEST--
Hash: PBKDF2 context reuse regression (GH-9604)
--FILE--
<?php

echo "*** Testing hash_pbkdf2() GH-9604 vectors ***\n";

$sha1Hex = hash_pbkdf2('sha1', 'password', 'salt', 2, 40, false);
$sha1RawHex = bin2hex(hash_pbkdf2('sha1', 'password', 'salt', 2, 20, true));

echo "sha1_hex: {$sha1Hex}\n";
echo "sha1_raw_hex: {$sha1RawHex}\n";
echo "sha1_match: " . ($sha1Hex === $sha1RawHex ? 'yes' : 'no') . "\n";

$sha256Hex = hash_pbkdf2('sha256', 'password', 'salt', 2, 64, false);
$sha256RawHex = bin2hex(hash_pbkdf2('sha256', 'password', 'salt', 2, 32, true));

echo "sha256_hex: {$sha256Hex}\n";
echo "sha256_raw_hex: {$sha256RawHex}\n";
echo "sha256_match: " . ($sha256Hex === $sha256RawHex ? 'yes' : 'no') . "\n";

echo "sha256_raw_hex_64: " . bin2hex(hash_pbkdf2('sha256', 'password', 'salt', 2, 64, true)) . "\n";

?>
--EXPECT--
*** Testing hash_pbkdf2() GH-9604 vectors ***
sha1_hex: ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957
sha1_raw_hex: ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957
sha1_match: yes
sha256_hex: ae4d0c95af6b46d32d0adff928f06dd02a303f8ef3c251dfd6e2d85a95474c43
sha256_raw_hex: ae4d0c95af6b46d32d0adff928f06dd02a303f8ef3c251dfd6e2d85a95474c43
sha256_match: yes
sha256_raw_hex_64: ae4d0c95af6b46d32d0adff928f06dd02a303f8ef3c251dfd6e2d85a95474c43830651afcb5c862f0b249bd031f7a67520d136470f5ec271ece91c07773253d9
