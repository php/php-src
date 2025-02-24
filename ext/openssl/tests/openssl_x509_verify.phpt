--TEST--
openssl_x509_verify() tests
--EXTENSIONS--
openssl
--FILE--
<?php

$certFile = __DIR__ . '/openssl-x509-verify-cert.pem.tmp';
$keyFile = __DIR__ . '/openssl-x509-verify-key.pem.tmp';

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator(true);
$certificateGenerator->saveNewCertAndPubKey('openssl-x509-verify-server', $certFile, $keyFile);

$fp = fopen($certFile,"r");
$a = fread($fp, 8192);
fclose($fp);

$fp = fopen($keyFile,"r");
$b = fread($fp, 8192);
fclose($fp);

$cert = "file://" . $certFile;
$key = "file://" . $keyFile;
$wrongKey = "file://" . __DIR__ . "/public_rsa_2048.key";

var_dump(openssl_x509_verify($cert, $key));
var_dump(openssl_x509_verify("", $key));
var_dump(openssl_x509_verify($cert, ""));
var_dump(openssl_x509_verify("", ""));
var_dump(openssl_x509_verify(openssl_x509_read($a), $b));
var_dump(openssl_x509_verify($cert, $wrongKey));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/openssl-x509-verify-cert.pem.tmp');
@unlink(__DIR__ . '/openssl-x509-verify-key.pem.tmp');
?>
--EXPECT--
int(1)
int(-1)
int(-1)
int(-1)
int(1)
int(0)
