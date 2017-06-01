--TEST--
openssl_pkcs7_verify() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$outfile = tempnam(sys_get_temp_dir(), "ssl");
if ($outfile === false) {
	die("failed to get a temporary filename!");
}

$contentfile = tempnam(sys_get_temp_dir(), "ssl");
if ($contentfile === false) {
	die("failed to get a temporary filename!");
}

$infile = dirname(__FILE__) . "/cert.crt";
$eml = dirname(__FILE__) . "/signed.eml";
$wrong = "wrong";
$empty = "";
$cainfo = array();

var_dump(openssl_pkcs7_verify($wrong, 0));
var_dump(openssl_pkcs7_verify($empty, 0));
var_dump(openssl_pkcs7_verify($eml, 0));
var_dump(openssl_pkcs7_verify($eml, 0, $empty));
var_dump(openssl_pkcs7_verify($eml, PKCS7_NOVERIFY, $outfile));
var_dump(openssl_pkcs7_verify($eml, PKCS7_NOVERIFY, $outfile, $cainfo, $outfile, $contentfile));

if (file_exists($outfile)) {
	echo "true\n";
	unlink($outfile);
}

if (file_exists($contentfile)) {
	echo "true\n";
	unlink($contentfile);
}
?>
--EXPECTF--
int(-1)
int(-1)
bool(false)
bool(false)
bool(true)
bool(true)
true
true
