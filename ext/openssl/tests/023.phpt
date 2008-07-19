--TEST--
openssl_pkcs7_encrypt() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$infile = dirname(__FILE__) . "/cert.crt";
$outfile = tempnam(b"/tmp", b"ssl");
if ($outfile === false)
	die("failed to get a temporary filename!");

$single_cert = "file://" . dirname(__FILE__) . "/cert.crt";
$multi_certs = array($single_cert, $single_cert);
$assoc_headers = array("To" => "test@test", "Subject" => "testing openssl_pkcs7_encrypt()");
$assoc_headers_bin = array(b"To" => b"test@test", b"Subject" => b"testing openssl_pkcs7_encrypt()");
$headers = array("test@test", "testing openssl_pkcs7_encrypt()");
$headers_bin = array(b"test@test", b"testing openssl_pkcs7_encrypt()");
$empty_headers = array();
$unicode_headers = array("\u0500" => "test", "test" => "invalid unicode\u0500");
$wrong = "wrong";
$wrong2 = b"wrong";
$empty = b"";

var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $headers_bin));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $assoc_headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $assoc_headers_bin));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $empty_headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $unicode_headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $wrong));
var_dump(openssl_pkcs7_encrypt($wrong, $outfile, $single_cert, $headers));
var_dump(openssl_pkcs7_encrypt($empty, $outfile, $single_cert, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $empty, $single_cert, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $wrong, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $wrong2, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $empty, $headers));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $single_cert, $empty));
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $multi_certs, $headers));

if (file_exists($outfile)) {
	echo "true\n";
	unlink($outfile);
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: openssl_pkcs7_encrypt(): Binary or ASCII-Unicode string expected, non-ASCII-Unicode string received. Skipping it. in %s on line %d

Warning: openssl_pkcs7_encrypt(): Binary or ASCII-Unicode string expected, non-ASCII-Unicode string received. Skipping it. in %s on line %d
bool(true)

Warning: openssl_pkcs7_encrypt() expects parameter 4 to be array, Unicode string given in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)

Warning: openssl_pkcs7_encrypt(): Binary string expected, Unicode string received in %s on line %d
bool(false)
bool(false)
bool(false)

Warning: openssl_pkcs7_encrypt() expects parameter 4 to be array, binary string given in %s on line %d
bool(false)
bool(true)
true
