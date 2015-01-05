--TEST--
openssl_x509_read() and openssl_x509_free() tests
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

var_dump($res = openssl_x509_read($a));         // read cert as a string
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($b));         // read cert as a filename string
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($c));         // read an invalid cert, fails
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($d));         // read cert from a resource
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($e));         // read an array
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($f));         // read an array with the filename
openssl_x509_free($res);
var_dump($res);
?>
--EXPECTF--
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (Unknown)
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (Unknown)

Warning: openssl_x509_read(): supplied parameter cannot be coerced into an X509 certificate! in %s on line %d
bool(false)

Warning: openssl_x509_free() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (Unknown)

Warning: openssl_x509_read(): supplied parameter cannot be coerced into an X509 certificate! in %s on line %d
bool(false)

Warning: openssl_x509_free() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)

Warning: openssl_x509_read(): supplied parameter cannot be coerced into an X509 certificate! in %s on line %d
bool(false)

Warning: openssl_x509_free() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)
