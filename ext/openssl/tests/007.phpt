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
$c = (binary) ("file://" . dirname(__FILE__) . "/cert.crt");
$d = "invalid cert";
$e = "non-ascii-unicode\u7000";	
$f = openssl_x509_read($a);
$g = (unicode) $a;

var_dump($res = openssl_x509_read($a));         // read cert as a binary string
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($b));         // read cert from a file, unicode string
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($c));         // read cert from a file, binary string
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($d));         // read an invalid cert, fails
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read((binary)$d));         // read an invalid cert, fails (binary)
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($e));         // read an invalid unicode string, fails
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($f));         // read cert from a resource
openssl_x509_free($res);
var_dump($res);
var_dump($res = openssl_x509_read($g));         // read cert as a unicode string
openssl_x509_free($res);
var_dump($res);

?>
--EXPECTF--
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (Unknown)
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (Unknown)
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (Unknown)

Warning: openssl_x509_read(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_x509_read(): supplied parameter cannot be coerced into an X509 certificate! in %s on line %d
bool(false)

Warning: openssl_x509_free() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)

Warning: openssl_x509_read(): supplied parameter cannot be coerced into an X509 certificate! in %s on line %d
bool(false)

Warning: openssl_x509_free() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)

Warning: openssl_x509_read(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_x509_read(): supplied parameter cannot be coerced into an X509 certificate! in %s on line %d
bool(false)

Warning: openssl_x509_free() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)
resource(%d) of type (OpenSSL X.509)
resource(%d) of type (Unknown)

Warning: openssl_x509_read(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_x509_read(): supplied parameter cannot be coerced into an X509 certificate! in %s on line %d
bool(false)

Warning: openssl_x509_free() expects parameter 1 to be resource, boolean given in %s on line %d
bool(false)
