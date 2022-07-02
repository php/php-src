--TEST--
openssl_pkcs12_export() tests
--EXTENSIONS--
openssl
--FILE--
<?php
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

var_dump(openssl_pkcs12_export($cert, $output, $priv, $pass)); // read certs as a string
var_dump(openssl_pkcs12_read($output, $opts, $pass));
var_dump(openssl_pkcs12_export($cert_path, $output, $priv_path, $pass)); // read certs from a filename string
var_dump(openssl_pkcs12_read($output, $opts, $pass));
var_dump(openssl_pkcs12_export($cert_res, $output, $priv_res, $pass)); // read certs from a resource
var_dump(openssl_pkcs12_read($output, $opts, $pass));
var_dump(openssl_pkcs12_export($cert, $output, $priv, $pass, array('extracerts' => $cert))); // extra optional cert
var_dump(openssl_pkcs12_read($output, $opts, $pass));
var_dump(count($opts)); // should be 3 certificates, priv, pub, extra optional cert


var_dump(openssl_pkcs12_export($invalid, $output, $invalid, $pass));
var_dump(openssl_pkcs12_export($invalid_path, $output, $invalid_path, $pass));
try {
    var_dump(openssl_pkcs12_export($priv_res, $output, $cert_res, $pass));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
//var_dump(openssl_pkcs12_export($cert, $output, $priv, $pass, array("foo")));
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
int(3)

Warning: openssl_pkcs12_export(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)

Warning: openssl_pkcs12_export(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)
openssl_pkcs12_export(): Argument #1 ($certificate) must be of type OpenSSLCertificate|string, OpenSSLAsymmetricKey given
