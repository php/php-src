--TEST--
openssl_cms_sign() tests
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

var_dump(openssl_cms_sign($infile, $outfile, openssl_x509_read($single_cert), $privkey, $headers));
$s = stat($outfile);
if ($s['size'] < 500) {
   print "size failure, not big enough.";
}
var_dump(openssl_cms_sign($infile, $outfile, $single_cert, $privkey, $headers));
var_dump(openssl_cms_sign($infile, $outfile, $single_cert, $privkey, $assoc_headers));
var_dump(openssl_cms_sign($infile, $outfile, $single_cert, $privkey, $empty_headers));
var_dump(openssl_cms_sign($wrong, $outfile, $single_cert, $privkey, $headers));
var_dump(openssl_cms_sign($empty, $outfile, $single_cert, $privkey, $headers));
var_dump(openssl_cms_sign($infile, $empty, $single_cert, $privkey, $headers));
var_dump(openssl_cms_sign($infile, $outfile, $wrong, $privkey, $headers));
var_dump(openssl_cms_sign($infile, $outfile, $empty, $privkey, $headers));
var_dump(openssl_cms_sign($infile, $outfile, $single_cert, $wrong, $headers));

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

Warning: openssl_cms_sign(): Error opening input file %s in %s on line %d
bool(false)

Warning: openssl_cms_sign(): Error opening input file %s in %s on line %d
bool(false)

Warning: openssl_cms_sign(): Error opening output file %s in %s on line %d
bool(false)

Warning: openssl_cms_sign(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)

Warning: openssl_cms_sign(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)

Warning: openssl_cms_sign(): Error getting private key in %s on line %d
bool(false)
true
