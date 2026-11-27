--TEST--
openssl_spki_export_challenge() preserves empty challenges and embedded NUL bytes
--EXTENSIONS--
openssl
--FILE--
<?php
$key = openssl_pkey_get_private('file://' . __DIR__ . '/private_rsa_2048.key');

foreach (["", "challenge", "\0challenge", "challenge\0", "left\0right"] as $challenge) {
    $spkac = openssl_spki_new($key, $challenge, OPENSSL_ALGO_SHA256);
    var_dump(openssl_spki_export_challenge(substr($spkac, 6)) === $challenge);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
