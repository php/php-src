--TEST--
openssl_x509_check_private_key() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$fp = fopen(dirname(__FILE__) . "/cert.crt","r");
$a = fread($fp, 8192);
fclose($fp);

$fp = fopen(dirname(__FILE__) . "/private_rsa_1024.key","r");
$b = fread($fp, 8192);
fclose($fp);

$cert = "file://" . dirname(__FILE__) . "/cert.crt";
$key = "file://" . dirname(__FILE__) . "/private_rsa_1024.key";

var_dump(openssl_x509_check_private_key($cert, $key));
var_dump(openssl_x509_check_private_key("", $key));
var_dump(openssl_x509_check_private_key($cert, ""));
var_dump(openssl_x509_check_private_key("", ""));
var_dump(openssl_x509_check_private_key($a, $b));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
