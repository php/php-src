--TEST--
openssl_pkcs12_export_to_file() error tests
--EXTENSIONS--
openssl
--FILE--
<?php
$pkcsfile = __DIR__ . "/openssl_pkcs12_export_to_file__pkcsfile.tmp";

$cert_file = __DIR__ . "/public.crt";
$cert = file_get_contents($cert_file);
$cert_path = "file://" . $cert_file;
$priv_file = __DIR__ . "/private.crt";
$priv = file_get_contents($priv_file);
$wrong_priv_file = __DIR__ . "/private_rsa_1024.key";
$wrong_priv = file_get_contents($wrong_priv_file);
$pass = 'test';

var_dump(openssl_pkcs12_export_to_file($cert, $pkcsfile, null, $pass));
var_dump(openssl_pkcs12_export_to_file($cert, $pkcsfile, $wrong_priv, $pass));
var_dump(openssl_pkcs12_export_to_file($cert, '.', $priv, $pass));
?>
--CLEAN--
<?php
$pkcsfile = __DIR__ . "/openssl_pkcs12_export_to_file__pkcsfile.tmp";
if (file_exists($pkcsfile)) {
    unlink($pkcsfile);
}
?>
--EXPECTF--
Warning: openssl_pkcs12_export_to_file(): Cannot get private key from parameter 3 in %s on line %d
bool(false)

Warning: openssl_pkcs12_export_to_file(): Private key does not correspond to cert in %s on line %d
bool(false)

Warning: openssl_pkcs12_export_to_file(): Error opening file %s in %s on line %d
bool(false)
