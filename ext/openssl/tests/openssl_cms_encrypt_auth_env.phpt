--TEST--
openssl_cms_encrypt() auth enveloped data tests
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (OPENSSL_VERSION_NUMBER < 0x30000000) die('skip For OpenSSL >= 3.0');
?>
--FILE--
<?php
$infile = __DIR__ . "/plain.txt";
$outfile = __DIR__ . "/openssl_cms_encrypt_auth_env.out1";
$outfile2 = __DIR__ . "/openssl_cms_encrypt_auth_env.out2";
$single_cert = "file://" . __DIR__ . "/cert.crt";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$headers = array("test@test", "testing openssl_cms_encrypt()");
$cipher = "AES-128-GCM";

var_dump(openssl_cms_encrypt($infile, $outfile, $single_cert, $headers, cipher_algo: $cipher));
var_dump(openssl_cms_encrypt($infile, $outfile, openssl_x509_read($single_cert), $headers, cipher_algo: $cipher));
var_dump(openssl_cms_decrypt($outfile, $outfile2, $single_cert, $privkey));
readfile($outfile2);

?>
--CLEAN--
<?php
@unlink(__DIR__ . "/openssl_cms_encrypt_auth_env.out1");
@unlink(__DIR__ . "/openssl_cms_encrypt_auth_env.out2");
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
Now is the winter of our discontent.
