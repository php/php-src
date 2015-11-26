--TEST--
openssl_pkey_export() with EC key
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!defined('OPENSSL_KEYTYPE_EC')) die("skip no EC available");
?>
--FILE--
<?php
$key = openssl_pkey_get_private('file://' . dirname(__FILE__) . '/private_ec.key');
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
$pKey = openssl_pkey_get_public('file://' . dirname(__FILE__) . '/public_ec.key');
var_dump($pKey);
// The details are the same for a public or private key
var_dump($details === openssl_pkey_get_details($pKey));


// Export to file
$tempname = tempnam(sys_get_temp_dir(), 'openssl_ec');
var_dump(openssl_pkey_export_to_file($key, $tempname, NULL, $config_arg));
$details = openssl_pkey_get_details(openssl_pkey_get_private('file://' . $tempname));
var_dump(OPENSSL_KEYTYPE_EC === $details['type']);

// Clean the temporary file
@unlink($tempname);

?>
--EXPECTF--
resource(%d) of type (OpenSSL key)
bool(true)
-----BEGIN EC PRIVATE KEY-----%a-----END EC PRIVATE KEY-----
bool(true)
bool(true)
resource(%d) of type (OpenSSL key)
bool(true)
bool(true)
bool(true)
