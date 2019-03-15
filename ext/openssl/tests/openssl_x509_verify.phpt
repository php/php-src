--TEST--
openssl_x509_verify() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$fp = fopen(__DIR__ . "/cert.crt","r");
$a = fread($fp, 8192);
fclose($fp);

$fp = fopen(__DIR__ . "/public.key","r");
$b = fread($fp, 8192);
fclose($fp);

$cert = "file://" . __DIR__ . "/cert.crt";
$key = "file://" . __DIR__ . "/public.key";
$wrongKey = "file://" . __DIR__ . "/public_rsa_2048.key";

var_dump(openssl_x509_verify($cert, $key));
var_dump(openssl_x509_verify("", $key));
var_dump(openssl_x509_verify($cert, ""));
var_dump(openssl_x509_verify("", ""));
var_dump(openssl_x509_verify(openssl_x509_read($a), $b));
var_dump(openssl_x509_verify($cert, $wrongKey));
?>
--EXPECT--
int(1)
int(-1)
int(-1)
int(-1)
int(1)
int(0)
