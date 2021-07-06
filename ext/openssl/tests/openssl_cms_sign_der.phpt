--TEST--
openssl_cms_sign() der tests
--EXTENSIONS--
openssl
--FILE--
<?php
$infile = __DIR__ . "/cert.crt";
$outfile = tempnam(sys_get_temp_dir(), "ssl");
if ($outfile === false) {
    die("failed to get a temporary filename!");
}

$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$single_cert = "file://" . __DIR__ . "/cert.crt";
$assoc_headers = array("To" => "test@test", "Subject" => "testing openssl_cms_sign()");
$headers = array("test@test", "testing openssl_cms_sign()");
$empty_headers = array();
$wrong = "wrong";
$empty = "";

var_dump(openssl_cms_sign($infile, $outfile, openssl_x509_read($single_cert), $privkey, $empty_headers, OPENSSL_CMS_DETACHED|OPENSSL_CMS_BINARY, OPENSSL_ENCODING_DER));
var_dump(openssl_cms_sign($infile, $outfile, openssl_x509_read($single_cert), $privkey, $headers));

if (file_exists($outfile)) {
    echo "true\n";
    unlink($outfile);
}
?>
--EXPECT--
bool(true)
bool(true)
true
