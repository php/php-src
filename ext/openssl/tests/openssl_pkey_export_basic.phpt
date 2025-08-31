--TEST--
openssl_pkey_export() with EC key
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined('OPENSSL_KEYTYPE_EC'))
    die("skip no EC available");
?>
--FILE--
<?php
$key = openssl_pkey_get_private('file://' . __DIR__ . '/private_ec.key');
var_dump($key);

$config_arg = array("config" => __DIR__ . DIRECTORY_SEPARATOR . "openssl.cnf");

var_dump(openssl_pkey_export($key, $output, NULL, $config_arg));
echo $output;

// Load the private key from the exported pem string
$details = openssl_pkey_get_details(openssl_pkey_get_private($output));
var_dump(OPENSSL_KEYTYPE_EC === $details['type']);

// Export key with passphrase
openssl_pkey_export($key, $output, 'passphrase', $config_arg);

$details = openssl_pkey_get_details(openssl_pkey_get_private($output, 'passphrase'));
var_dump(OPENSSL_KEYTYPE_EC === $details['type']);

// Read public key
$pKey = openssl_pkey_get_public('file://' . __DIR__ . '/public_ec.key');
var_dump($pKey);
// The details are the same for a public or private key, expect the private key parameter 'd
$detailsPKey = openssl_pkey_get_details($pKey);
var_dump(array_diff_assoc($details['ec'], $detailsPKey['ec']));

// Export to file
$tempname = tempnam(sys_get_temp_dir(), 'openssl_ec');
var_dump(openssl_pkey_export_to_file($key, $tempname, NULL, $config_arg));
$details = openssl_pkey_get_details(openssl_pkey_get_private('file://' . $tempname));
var_dump(OPENSSL_KEYTYPE_EC === $details['type']);
var_dump($key instanceof OpenSSLAsymmetricKey);
// Clean the temporary file
@unlink($tempname);
?>
--EXPECTF--
object(OpenSSLAsymmetricKey)#%d (0) {
}
bool(true)
-----BEGIN PRIVATE KEY-----
MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgs+Sqh7IzteDBiS5K
PfTvuWuyt9YkrkuoyiW/6bag6NmhRANCAAQ+riFshYe8HnWt1avx6OuNajipU1ZW
6BgW0+D/EtDDSYeQg9ngO8qyo5M6cyh7ORtKZVUy7DP1+W+eocaZC+a6
-----END PRIVATE KEY-----
bool(true)
bool(true)
object(OpenSSLAsymmetricKey)#%d (0) {
}
array(1) {
  ["d"]=>
  string(32) "%a"
}
bool(true)
bool(true)
bool(true)
