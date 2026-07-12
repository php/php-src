--TEST--
openssl_pkey_new() can construct a public-only RSA key from n and e
--EXTENSIONS--
openssl
--FILE--
<?php
// Generate a private key, then rebuild a public-only key from its raw n and e.
$private = openssl_pkey_new([
    'private_key_type' => OPENSSL_KEYTYPE_RSA,
    'private_key_bits' => 2048,
]);
$priv_details = openssl_pkey_get_details($private);
$n = $priv_details['rsa']['n'];
$e = $priv_details['rsa']['e'];

$public = openssl_pkey_new(['rsa' => ['n' => $n, 'e' => $e]]);
var_dump($public instanceof OpenSSLAsymmetricKey);

$pub_details = openssl_pkey_get_details($public);
var_dump($pub_details['type'] === OPENSSL_KEYTYPE_RSA);
var_dump($pub_details['bits']);
// A public-only key exposes n and e but not the private components.
var_dump(isset($pub_details['rsa']['n']));
var_dump(isset($pub_details['rsa']['e']));
var_dump(isset($pub_details['rsa']['d']));
var_dump(strpos($pub_details['key'], 'BEGIN PUBLIC KEY') !== false);
var_dump($pub_details['rsa']['n'] === $n);
var_dump($pub_details['rsa']['e'] === $e);

// The reconstructed public key can verify a signature made by the private key.
$data = 'The quick brown fox';
openssl_sign($data, $signature, $private, OPENSSL_ALGO_SHA256);
var_dump(openssl_verify($data, $signature, $public, OPENSSL_ALGO_SHA256));
var_dump(openssl_verify('tampered', $signature, $public, OPENSSL_ALGO_SHA256));

// It can also be used for public-key encryption.
openssl_public_encrypt('secret', $encrypted, $public);
openssl_private_decrypt($encrypted, $decrypted, $private);
var_dump($decrypted === 'secret');

// Missing e (and d) is still an error: a public key needs the exponent.
var_dump(@openssl_pkey_new(['rsa' => ['n' => $n]]));
?>
--EXPECT--
bool(true)
bool(true)
int(2048)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
int(1)
int(0)
bool(true)
bool(false)
