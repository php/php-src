--TEST--
openssl_x509_check_private_key() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$fp = fopen(dirname(__FILE__) . "/cert.crt","r");
$a = fread($fp,8192);
fclose($fp);

$fp = fopen(dirname(__FILE__) . "/private.key","r");
$b = fread($fp,8192);
fclose($fp);

$cert = "file://" . dirname(__FILE__) . "/cert.crt";
$key = "file://" . dirname(__FILE__) . "/private.key";

var_dump(openssl_x509_check_private_key($cert, $key));
var_dump(openssl_x509_check_private_key(b"", $key));
var_dump(openssl_x509_check_private_key($cert, b""));
var_dump(openssl_x509_check_private_key(b"", b""));
var_dump(openssl_x509_check_private_key($a, $b));
var_dump(openssl_x509_check_private_key((unicode)$a, $b));		// fails, should be binary only
var_dump(openssl_x509_check_private_key((unicode)$a, (unicode)$b));	// fails, should be binary only
var_dump(openssl_x509_check_private_key($a, (unicode)$b));		// fails, should be binary only
?>
--EXPECTF--
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)

Warning: openssl_x509_check_private_key(): Binary string expected, Unicode string received in %s on line %d
bool(false)

Warning: openssl_x509_check_private_key(): Binary string expected, Unicode string received in %s on line %d
bool(false)

Warning: openssl_x509_check_private_key(): Binary string expected, Unicode string received in %s on line %d
bool(false)
