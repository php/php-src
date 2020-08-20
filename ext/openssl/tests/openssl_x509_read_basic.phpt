--TEST--
openssl_x509_read() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$fp = fopen(__DIR__ . "/cert.crt","r");
$a = fread($fp,8192);
fclose($fp);

$b = "file://" . __DIR__ . "/cert.crt";
$c = "invalid cert";
$d = openssl_x509_read($a);
$e = array();
$f = array($b);

var_dump(openssl_x509_read($a)); // read cert as a string
var_dump(openssl_x509_read($b)); // read cert as a filename string
var_dump(openssl_x509_read($c)); // read an invalid cert, fails
var_dump(openssl_x509_read($d)); // read cert from a resource

try {
    openssl_x509_read($e); // read an array
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    openssl_x509_read($f); // read an array with the filename
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECTF--
object(OpenSSLCertificate)#%d (0) {
}
object(OpenSSLCertificate)#%d (0) {
}

Warning: openssl_x509_read(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)
object(OpenSSLCertificate)#%d (0) {
}
openssl_x509_read(): Argument #1 ($certificate) must be of type OpenSSLCertificate|string, array given
openssl_x509_read(): Argument #1 ($certificate) must be of type OpenSSLCertificate|string, array given
