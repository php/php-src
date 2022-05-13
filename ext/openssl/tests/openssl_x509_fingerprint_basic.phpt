--TEST--
openssl_x509_fingerprint() tests
--EXTENSIONS--
openssl
--FILE--
<?php

$cert = "file://" . __DIR__ . "/cert.crt";

echo "** Testing default functionality **\n";
var_dump(openssl_x509_fingerprint($cert));

echo "** Testing hash method md5 **\n";
var_dump(openssl_x509_fingerprint($cert, 'md5'));

echo "**Testing raw output md5 **\n";
var_dump(bin2hex(openssl_x509_fingerprint($cert, 'md5', true)));

echo "** Testing hash method sha1 with resource **\n";
$r = openssl_x509_read($cert);
var_dump(openssl_x509_fingerprint($r, 'sha1'));

echo "** Testing bad certification **\n";
var_dump(openssl_x509_fingerprint('123'));
echo "** Testing bad hash method **\n";
var_dump(openssl_x509_fingerprint($cert, 'xx45'));
?>
--EXPECTF--
** Testing default functionality **
string(40) "6e6fd1ea10a5a23071d61c728ee9b40df6dbc33c"
** Testing hash method md5 **
string(32) "ac77008e172897e06c0b065294487a67"
**Testing raw output md5 **
string(32) "ac77008e172897e06c0b065294487a67"
** Testing hash method sha1 with resource **
string(40) "6e6fd1ea10a5a23071d61c728ee9b40df6dbc33c"
** Testing bad certification **

Warning: openssl_x509_fingerprint(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)
** Testing bad hash method **

Warning: openssl_x509_fingerprint(): Unknown digest algorithm in %s on line %d
bool(false)
