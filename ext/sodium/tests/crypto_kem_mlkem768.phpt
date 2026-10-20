--TEST--
Check for libsodium crypto_kem_mlkem768
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_KEM_MLKEM768_PUBLICKEYBYTES')) print "skip libsodium without ML-KEM768 support (requires >= 1.0.22)";
?>
--FILE--
<?php
echo "crypto_kem_mlkem768:\n";

/* Constant sanity */
var_dump(SODIUM_CRYPTO_KEM_MLKEM768_KEYPAIRBYTES ===
         SODIUM_CRYPTO_KEM_MLKEM768_SECRETKEYBYTES + SODIUM_CRYPTO_KEM_MLKEM768_PUBLICKEYBYTES);
var_dump(SODIUM_CRYPTO_KEM_MLKEM768_SHAREDSECRETBYTES === 32);

/* Round-trip: keypair -> extractors -> enc -> dec */
$keypair = sodium_crypto_kem_mlkem768_keypair();
var_dump(strlen($keypair) === SODIUM_CRYPTO_KEM_MLKEM768_KEYPAIRBYTES);
$secret_key = sodium_crypto_kem_mlkem768_secretkey($keypair);
$public_key = sodium_crypto_kem_mlkem768_publickey($keypair);
var_dump(strlen($secret_key) === SODIUM_CRYPTO_KEM_MLKEM768_SECRETKEYBYTES);
var_dump(strlen($public_key) === SODIUM_CRYPTO_KEM_MLKEM768_PUBLICKEYBYTES);

[$ciphertext, $shared_secret] = sodium_crypto_kem_mlkem768_enc($public_key);
var_dump(strlen($ciphertext) === SODIUM_CRYPTO_KEM_MLKEM768_CIPHERTEXTBYTES);
var_dump(strlen($shared_secret) === SODIUM_CRYPTO_KEM_MLKEM768_SHAREDSECRETBYTES);
var_dump(sodium_crypto_kem_mlkem768_dec($ciphertext, $secret_key) === $shared_secret);

/* Encapsulation is randomized: same public key, fresh ciphertext and secret */
[$ciphertext2, $shared_secret2] = sodium_crypto_kem_mlkem768_enc($public_key);
var_dump($ciphertext2 !== $ciphertext);
var_dump($shared_secret2 !== $shared_secret);
var_dump(sodium_crypto_kem_mlkem768_dec($ciphertext2, $secret_key) === $shared_secret2);

/* Key generation is randomized */
var_dump(sodium_crypto_kem_mlkem768_keypair() !== $keypair);

/* seed_keypair is deterministic and round-trips */
$seed = random_bytes(SODIUM_CRYPTO_KEM_MLKEM768_SEEDBYTES);
$seeded_keypair = sodium_crypto_kem_mlkem768_seed_keypair($seed);
var_dump(strlen($seeded_keypair) === SODIUM_CRYPTO_KEM_MLKEM768_KEYPAIRBYTES);
var_dump(sodium_crypto_kem_mlkem768_seed_keypair($seed) === $seeded_keypair);
[$seeded_ciphertext, $seeded_shared_secret] =
    sodium_crypto_kem_mlkem768_enc(sodium_crypto_kem_mlkem768_publickey($seeded_keypair));
var_dump(sodium_crypto_kem_mlkem768_dec($seeded_ciphertext,
                                        sodium_crypto_kem_mlkem768_secretkey($seeded_keypair)) === $seeded_shared_secret);

/* Implicit rejection: a tampered ciphertext still decapsulates to a
 * shared secret of the right length, but not the original one */
$tampered_ciphertext = $ciphertext;
$tampered_ciphertext[0] = $tampered_ciphertext[0] === "\x00" ? "\x01" : "\x00";
$tampered_shared_secret = sodium_crypto_kem_mlkem768_dec($tampered_ciphertext, $secret_key);
var_dump(strlen($tampered_shared_secret) === SODIUM_CRYPTO_KEM_MLKEM768_SHAREDSECRETBYTES);
var_dump($tampered_shared_secret !== $shared_secret);

/* Error: truncated public key */
try {
    sodium_crypto_kem_mlkem768_enc(substr($public_key, 0, -1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
/* Error: truncated ciphertext */
try {
    sodium_crypto_kem_mlkem768_dec(substr($ciphertext, 0, -1), $secret_key);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
/* Error: truncated secret key */
try {
    sodium_crypto_kem_mlkem768_dec($ciphertext, substr($secret_key, 0, -1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
/* Error: truncated keypair */
try {
    sodium_crypto_kem_mlkem768_secretkey(substr($keypair, 0, -1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
/* Error: overlong keypair */
try {
    sodium_crypto_kem_mlkem768_publickey($keypair . 'x');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
/* Error: wrong-length seed */
try {
    sodium_crypto_kem_mlkem768_seed_keypair(random_bytes(SODIUM_CRYPTO_KEM_MLKEM768_SEEDBYTES - 1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
/* Error: a public key with an invalid ML-KEM encoding is rejected */
try {
    sodium_crypto_kem_mlkem768_enc(str_repeat("\xff", SODIUM_CRYPTO_KEM_MLKEM768_PUBLICKEYBYTES));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
crypto_kem_mlkem768:
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
SodiumException: sodium_crypto_kem_mlkem768_enc(): Argument #1 ($public_key) must be SODIUM_CRYPTO_KEM_MLKEM768_PUBLICKEYBYTES bytes long
SodiumException: sodium_crypto_kem_mlkem768_dec(): Argument #1 ($ciphertext) must be SODIUM_CRYPTO_KEM_MLKEM768_CIPHERTEXTBYTES bytes long
SodiumException: sodium_crypto_kem_mlkem768_dec(): Argument #2 ($secret_key) must be SODIUM_CRYPTO_KEM_MLKEM768_SECRETKEYBYTES bytes long
SodiumException: sodium_crypto_kem_mlkem768_secretkey(): Argument #1 ($key_pair) must be SODIUM_CRYPTO_KEM_MLKEM768_KEYPAIRBYTES bytes long
SodiumException: sodium_crypto_kem_mlkem768_publickey(): Argument #1 ($key_pair) must be SODIUM_CRYPTO_KEM_MLKEM768_KEYPAIRBYTES bytes long
SodiumException: sodium_crypto_kem_mlkem768_seed_keypair(): Argument #1 ($seed) must be SODIUM_CRYPTO_KEM_MLKEM768_SEEDBYTES bytes long
SodiumException: internal error
