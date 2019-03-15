--TEST--
openssl_pkcs7_encrypt() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$infile = __DIR__ . "/cert.crt";
$outfile = tempnam(sys_get_temp_dir(), "ssl");
if ($outfile === false)
    die("failed to get a temporary filename!");
$outfile2 = tempnam(sys_get_temp_dir(), "ssl");
if ($outfile2 === false)
    die("failed to get a temporary filename!");

$single_cert = "file://" . __DIR__ . "/cert.crt";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$multi_certs = array($single_cert, $single_cert);
$assoc_headers = array("To" => "test@test", "Subject" => "testing openssl_pkcs7_encrypt()");
$headers = array("test@test", "testing openssl_pkcs7_encrypt()");
$empty_headers = array();
$wrong = "wrong";
$empty = "";

var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, openssl_x509_read($single_cert), $headers));
var_dump(openssl_pkcs7_decrypt($outfile, $outfile2, $single_cert, $privkey));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $assoc_headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $empty_headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $wrong));
var_dump(openssl_pkcs7_encrypt($wrong, $outfile, $single_cert, $headers));
var_dump(openssl_pkcs7_encrypt($empty, $outfile, $single_cert, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $empty, $single_cert, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $wrong, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $empty, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $empty));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $multi_certs, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, array_map('openssl_x509_read', $multi_certs) , $headers));

if (file_exists($outfile)) {
    echo "true\n";
    unlink($outfile);
}
if (file_exists($outfile2)) {
    echo "true\n";
    unlink($outfile2);
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: openssl_pkcs7_encrypt() expects parameter 4 to be array, string given in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)

Warning: openssl_pkcs7_encrypt() expects parameter 4 to be array, string given in %s on line %d
bool(false)
bool(true)
bool(true)
true
true
