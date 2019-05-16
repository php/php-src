--TEST--
openssl_x509_check_private_key() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$fp = fopen(__DIR__ . "/cert.crt","r");
$a = fread($fp, 8192);
fclose($fp);

$fp = fopen(__DIR__ . "/private_rsa_1024.key","r");
$b = fread($fp, 8192);
fclose($fp);

$cert = "file://" . __DIR__ . "/cert.crt";
$key = "file://" . __DIR__ . "/private_rsa_1024.key";

var_dump(openssl_x509_check_private_key($cert, $key));
var_dump(openssl_x509_check_private_key("", $key));
var_dump(openssl_x509_check_private_key($cert, ""));
var_dump(openssl_x509_check_private_key("", ""));
var_dump(openssl_x509_check_private_key(openssl_x509_read($a), $b));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
