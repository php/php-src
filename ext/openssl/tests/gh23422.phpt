--TEST--
GH-23422 (openssl_sign()/openssl_verify() cannot sign/verify a precomputed digest)
--EXTENSIONS--
openssl
--FILE--
<?php
$conf = ['config' => __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf'];
$privkey = openssl_pkey_new($conf);
$pubkey = openssl_pkey_get_details($privkey)['key'];

$data = "Testing openssl_sign() with a precomputed digest";
$digest = hash('sha256', $data, true);

// Signing the data the normal way hashes it internally with SHA-256 before
// signing. Signing the precomputed digest directly (data_is_digest = true)
// must produce the exact same PKCS#1 v1.5 signature, since both ultimately
// sign the same DigestInfo(SHA-256, digest) structure.
var_dump(openssl_sign($data, $sign_from_data, $privkey, OPENSSL_ALGO_SHA256));
var_dump(openssl_sign($digest, $sign_from_digest, $privkey, OPENSSL_ALGO_SHA256, 0, OPENSSL_RSA_PSS_SALTLEN_AUTO, true));
var_dump($sign_from_data === $sign_from_digest);

// A signature produced from the precomputed digest must verify both against
// the precomputed digest and against the original data.
var_dump(openssl_verify($digest, $sign_from_digest, $pubkey, OPENSSL_ALGO_SHA256, 0, OPENSSL_RSA_PSS_SALTLEN_AUTO, true));
var_dump(openssl_verify($data, $sign_from_digest, $pubkey, OPENSSL_ALGO_SHA256));

// A tampered digest must fail verification instead of silently succeeding.
$tampered_digest = $digest;
$tampered_digest[0] = chr(ord($digest[0]) ^ 1);
var_dump(openssl_verify($tampered_digest, $sign_from_digest, $pubkey, OPENSSL_ALGO_SHA256, 0, OPENSSL_RSA_PSS_SALTLEN_AUTO, true));

// RSA-PSS padding is also honored on the precomputed-digest code path.
var_dump(openssl_sign($digest, $sign_pss, $privkey, OPENSSL_ALGO_SHA256, OPENSSL_PKCS1_PSS_PADDING, OPENSSL_RSA_PSS_SALTLEN_DIGEST, true));
var_dump(openssl_verify($digest, $sign_pss, $pubkey, OPENSSL_ALGO_SHA256, OPENSSL_PKCS1_PSS_PADDING, OPENSSL_RSA_PSS_SALTLEN_DIGEST, true));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
int(1)
int(1)
int(0)
bool(true)
int(1)
