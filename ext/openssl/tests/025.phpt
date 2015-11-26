--TEST--
openssl_pkcs7_sign() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$infile = dirname(__FILE__) . "/cert.crt";
$outfile = tempnam("/tmp", "ssl");
if ($outfile === false)
	die("failed to get a temporary filename!");

$privkey = "file://" . dirname(__FILE__) . "/private.key";
$single_cert = "file://" . dirname(__FILE__) . "/cert.crt";
$assoc_headers = array("To" => "test@test", "Subject" => "testing openssl_pkcs7_sign()");
$headers = array("test@test", "testing openssl_pkcs7_sign()");
$empty_headers = array();
$wrong = "wrong";
$empty = "";

var_dump(openssl_pkcs7_sign($infile, $outfile, $single_cert, $privkey, $headers));
var_dump(openssl_pkcs7_sign($infile, $outfile, $single_cert, $privkey, $assoc_headers));
var_dump(openssl_pkcs7_sign($infile, $outfile, $single_cert, $privkey, $empty_headers));
var_dump(openssl_pkcs7_sign($infile, $outfile, $single_cert, $privkey, $wrong));
var_dump(openssl_pkcs7_sign($wrong, $outfile, $single_cert, $privkey, $headers));
var_dump(openssl_pkcs7_sign($empty, $outfile, $single_cert, $privkey, $headers));
var_dump(openssl_pkcs7_sign($infile, $empty, $single_cert, $privkey, $headers));
var_dump(openssl_pkcs7_sign($infile, $outfile, $wrong, $privkey, $headers));
var_dump(openssl_pkcs7_sign($infile, $outfile, $empty, $privkey, $headers));
var_dump(openssl_pkcs7_sign($infile, $outfile, $single_cert, $privkey, $empty));
var_dump(openssl_pkcs7_sign($infile, $outfile, $single_cert, $wrong, $headers));

if (file_exists($outfile)) {
	echo "true\n";
	unlink($outfile);
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)

Warning: openssl_pkcs7_sign() expects parameter 5 to be array, string given in %s on line %d
NULL

Warning: openssl_pkcs7_sign(): error opening input file %s in %s on line %d
bool(false)

Warning: openssl_pkcs7_sign(): error opening input file %s in %s on line %d
bool(false)

Warning: openssl_pkcs7_sign(): error opening output file %s in %s on line %d
bool(false)

Warning: openssl_pkcs7_sign(): error getting cert in %s on line %d
bool(false)

Warning: openssl_pkcs7_sign(): error getting cert in %s on line %d
bool(false)

Warning: openssl_pkcs7_sign() expects parameter 5 to be array, string given in %s on line %d
NULL

Warning: openssl_pkcs7_sign(): error getting private key in %s on line %d
bool(false)
true
