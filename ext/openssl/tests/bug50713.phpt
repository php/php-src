--TEST--
Bug #50713 (openssl_pkcs7_verify() may ignore untrusted CAs)
--EXTENSIONS--
openssl
--FILE--
<?php
$inFile =  __DIR__ . "/plain.txt";
$outFile = __DIR__ . '/bug50713-out.pem';
$signersFile = __DIR__ . '/bug50713-signers.pem';
$keyFile = __DIR__ . '/bug50713-key.pem';
$certFile = __DIR__ . '/bug50713-crt.pem';
$cacertFile = __DIR__ . '/bug50713-ca.pem';

include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert($cacertFile);
$certificateGenerator->saveNewCertAndKey('bug50713', $certFile, $keyFile, 1024);

var_dump(openssl_pkcs7_sign($inFile, $outFile, 'file://' . $certFile, 'file://' . $keyFile, [], PKCS7_NOCERTS));
var_dump(openssl_pkcs7_verify($outFile, 0, $signersFile, [$cacertFile], $certFile));
var_dump(strlen(file_get_contents($signersFile)) > 0);
?>
--CLEAN--
<?php
$outFile = __DIR__ . '/bug50713-out.pem';
$signersFile = __DIR__ . '/bug50713-signers.pem';
$keyFile = __DIR__ . '/bug50713-key.pem';
$certFile = __DIR__ . '/bug50713-crt.pem';
$cacertFile = __DIR__ . '/bug50713-ca.pem';

@unlink($signersFile);
@unlink($outFile);
@unlink($keyFile);
@unlink($certFile);
@unlink($cacertFile);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
