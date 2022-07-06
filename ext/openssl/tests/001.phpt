--TEST--
OpenSSL private key functions
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!@openssl_pkey_new()) die("skip cannot create private key");
?>
--FILE--
<?php
echo "Creating private key\n";

$conf = array('config' => __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf');
$privkey = openssl_pkey_new($conf);

if ($privkey === false) {
    die("failed to create private key");
}

$passphrase = "banana";
$key_file_name = __DIR__ . '/001-tmp.key';
if ($key_file_name === false) {
    die("failed to get a temporary filename!");
}

echo "Export key to file\n";

if (!openssl_pkey_export_to_file($privkey, $key_file_name, $passphrase, $conf)) {
    die("failed to export to file $key_file_name");
}
var_dump($privkey instanceof OpenSSLAsymmetricKey);

echo "Load key from file - array syntax\n";

$loaded_key = openssl_pkey_get_private(array("file://$key_file_name", $passphrase));

if ($loaded_key === false) {
    die("failed to load key using array syntax");
}

openssl_pkey_free($loaded_key);

echo "Load key using direct syntax\n";

$loaded_key = openssl_pkey_get_private("file://$key_file_name", $passphrase);

if ($loaded_key === false) {
    die("failed to load key using direct syntax");
}

openssl_pkey_free($loaded_key);

echo "Load key manually and use string syntax\n";

$key_content = file_get_contents($key_file_name);
$loaded_key = openssl_pkey_get_private($key_content, $passphrase);

if ($loaded_key === false) {
    die("failed to load key using string syntax");
}
openssl_pkey_free($loaded_key);

echo "OK!\n";

?>
--CLEAN--
<?php
$key_file_name = __DIR__ . DIRECTORY_SEPARATOR . '001-tmp.key';
@unlink($key_file_name);
?>
--EXPECTF--
Creating private key
Export key to file
bool(true)
Load key from file - array syntax

Deprecated: Function openssl_pkey_free() is deprecated in %s on line %d
Load key using direct syntax

Deprecated: Function openssl_pkey_free() is deprecated in %s on line %d
Load key manually and use string syntax

Deprecated: Function openssl_pkey_free() is deprecated in %s on line %d
OK!
