--TEST--
openssl_pkcs12_export_to_file() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$pkcsfile = __DIR__ . "/openssl_pkcs12_export_to_file__pkcsfile.tmp";

$cert_file = __DIR__ . "/public.crt";
$cert = file_get_contents($cert_file);
$cert_path = "file://" . $cert_file;
$priv_file = __DIR__ . "/private.crt";
$priv = file_get_contents($priv_file);
$priv_path = "file://" . $priv_file;
$cert_res = openssl_x509_read($cert);
$priv_res = openssl_pkey_get_private($priv);
$pass = "test";
$invalid = "";
$invalid_path = __DIR__ . "/invalid_path";
$opts = [];

var_dump(openssl_pkcs12_export_to_file($cert, $pkcsfile, $priv, $pass));
var_dump(openssl_pkcs12_read(file_get_contents($pkcsfile), $opts, $pass));
var_dump(openssl_pkcs12_export_to_file($cert_path, $pkcsfile, $priv_path, $pass));
var_dump(openssl_pkcs12_read(file_get_contents($pkcsfile), $opts, $pass));
var_dump(openssl_pkcs12_export_to_file($cert_res, $pkcsfile, $priv_res, $pass));
var_dump(openssl_pkcs12_read(file_get_contents($pkcsfile), $opts, $pass));
var_dump(openssl_pkcs12_export_to_file($cert_res, $pkcsfile, $priv_res, $pass, array('extracerts' => $cert)));
var_dump(openssl_pkcs12_read(file_get_contents($pkcsfile), $opts, $pass));

var_dump(openssl_pkcs12_export_to_file($invalid, $pkcsfile, $invalid, $pass));
var_dump(openssl_pkcs12_export_to_file($invalid_path, $pkcsfile, $invalid_path, $pass));
try {
    var_dump(openssl_pkcs12_export_to_file($priv_res, $pkcsfile, $cert_res, $pass));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
$pkcsfile = __DIR__ . "/openssl_pkcs12_export_to_file__pkcsfile.tmp";
if (file_exists($pkcsfile)) {
    unlink($pkcsfile);
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: openssl_pkcs12_export_to_file(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)

Warning: openssl_pkcs12_export_to_file(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)
openssl_pkcs12_export_to_file(): Argument #1 ($certificate) must be of type OpenSSLCertificate|string, OpenSSLAsymmetricKey given
