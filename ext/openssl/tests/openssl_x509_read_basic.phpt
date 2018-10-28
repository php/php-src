--TEST--
openssl_x509_read() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$fp = fopen(dirname(__FILE__) . "/cert.crt","r");
$a = fread($fp,8192);
fclose($fp);

$b = "file://" . dirname(__FILE__) . "/cert.crt";
$c = "invalid cert";
$d = openssl_x509_read($a);
$e = array();
$f = array($b);

var_dump(openssl_x509_read($a)); // read cert as a string
var_dump(openssl_x509_read($b)); // read cert as a filename string
var_dump(openssl_x509_read($c)); // read an invalid cert, fails
var_dump(openssl_x509_read($d)); // read cert from a resource
var_dump(openssl_x509_read($e)); // read an array
var_dump(openssl_x509_read($f)); // read an array with the filename
?>
--EXPECTF--
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (OpenSSL X.509)

Warning: openssl_x509_read(): supplied parameter cannot be coerced into an X509 certificate! in %s on line %d
bool(false)
resource(%d) of type (OpenSSL X.509)

Warning: openssl_x509_read(): supplied parameter cannot be coerced into an X509 certificate! in %s on line %d
bool(false)

Warning: openssl_x509_read(): supplied parameter cannot be coerced into an X509 certificate! in %s on line %d
bool(false)
