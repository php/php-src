--TEST--
OpenSSL private key functions
--SKIPIF--
<?php // vim600:syn=php
include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php
echo "Creating private key\n";

/* stack up some entropy; performance is not critical,
 * and being slow will most likely even help the test.
 */
for ($z = "", $i = 0; $i < 1024; $i++) {
	$z .= $i * $i;
	if (function_exists("usleep"))
		usleep($i);
}

$privkey = openssl_pkey_new();

if ($privkey === false)
	die("failed to create private key");

$passphrase = "banana";
$key_file_name = tempnam("/tmp", "ssl");
if ($key_file_name === false)
	die("failed to get a temporary filename!");

echo "Export key to file\n";

openssl_pkey_export_to_file($privkey, $key_file_name, $passphrase) or die("failed to export to file $key_file_name");

echo "Load key from file - array syntax\n";

$loaded_key = openssl_pkey_get_private(array("file://$key_file_name", $passphrase));

if ($loaded_key === false)
	die("failed to load key using array syntax");

openssl_pkey_free($loaded_key);

echo "Load key using direct syntax\n";

$loaded_key = openssl_pkey_get_private("file://$key_file_name", $passphrase);

if ($loaded_key === false)
	die("failed to load key using direct syntax");

openssl_pkey_free($loaded_key);

echo "Load key manually and use string syntax\n";

$key_content = file_get_contents($key_file_name);
$loaded_key = openssl_pkey_get_private($key_content, $passphrase);

if ($loaded_key === false)
	die("failed to load key using string syntax");

openssl_pkey_free($loaded_key);

echo "OK!\n";

@unlink($key_file_name);

?>
--EXPECT--
Creating private key
Export key to file
Load key from file - array syntax
Load key using direct syntax
Load key manually and use string syntax
OK!
