--TEST--
Memory leak on sodium_crypto_sign_ed25519_pk_to_curve25519() failure
--EXTENSIONS--
sodium
--FILE--
<?php

try {
    sodium_crypto_sign_ed25519_pk_to_curve25519(str_repeat("\x00", SODIUM_CRYPTO_SIGN_PUBLICKEYBYTES));
} catch (SodiumException $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
conversion failed
