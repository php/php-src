--TEST--
Test hash_pbkdf2() function : basic functionality
--SKIPIF--
<?php extension_loaded('hash') or die('skip: hash extension not loaded.'); ?>
--FILE--
<?php

/* Prototype  : string hash_hmac(string $algo, string $data, string $key [, int $length = 0 [, bool $raw_output = false]])
 * Description: Generate a keyed hash value using the HMAC method
 * Source code: ext/hash/hash.c
 * Alias to functions:
*/

echo "*** Testing hash_pbkdf2() : basic functionality ***\n";

echo "sha1: " . hash_pbkdf2('sha1', 'password', 'salt', 1, 20)."\n";
echo "sha1(no length): " . hash_pbkdf2('sha1', 'password', 'salt', 1)."\n";
echo "sha1(raw): " . bin2hex(hash_pbkdf2('sha1', 'password', 'salt', 1, 20, TRUE))."\n";
echo "sha1(rounds): " . hash_pbkdf2('sha1', 'passwordPASSWORDpassword', 'saltSALTsaltSALTsaltSALTsaltSALTsalt', 4096, 25)."\n";
echo "sha1(rounds)(raw): " . bin2hex(hash_pbkdf2('sha1', 'passwordPASSWORDpassword', 'saltSALTsaltSALTsaltSALTsaltSALTsalt', 4096, 25, TRUE))."\n";
echo "sha256: " . hash_pbkdf2('sha256', 'password', 'salt', 1, 20)."\n";
echo "sha256(no length): " . hash_pbkdf2('sha256', 'password', 'salt', 1)."\n";
echo "sha256(raw): " . bin2hex(hash_pbkdf2('sha256', 'password', 'salt', 1, 20, TRUE))."\n";
echo "sha256(rounds): " . hash_pbkdf2('sha256', 'passwordPASSWORDpassword', 'saltSALTsaltSALTsaltSALTsaltSALTsalt', 4096, 40)."\n";
echo "sha256(rounds)(raw): " . bin2hex(hash_pbkdf2('sha256', 'passwordPASSWORDpassword', 'saltSALTsaltSALTsaltSALTsaltSALTsalt', 4096, 40, TRUE))."\n";

?>
===Done===
--EXPECT--
*** Testing hash_pbkdf2() : basic functionality ***
sha1: 0c60c80f961f0e71f3a9
sha1(no length): 0c60c80f961f0e71f3a9b524af6012062fe037a6
sha1(raw): 0c60c80f961f0e71f3a9b524af6012062fe037a6
sha1(rounds): 3d2eec4fe41c849b80c8d8366
sha1(rounds)(raw): 3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038
sha256: 120fb6cffcf8b32c43e7
sha256(no length): 120fb6cffcf8b32c43e7225256c4f837a86548c92ccc35480805987cb70be17b
sha256(raw): 120fb6cffcf8b32c43e7225256c4f837a86548c9
sha256(rounds): 348c89dbcbd32b2f32d814b8116e84cf2b17347e
sha256(rounds)(raw): 348c89dbcbd32b2f32d814b8116e84cf2b17347ebc1800181c4e2a1fb8dd53e1c635518c7dac47e9
===Done===
