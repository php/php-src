--TEST--
openssl_cms_sign() and verify detached tests
--EXTENSIONS--
openssl
--FILE--
<?php
$infile = __DIR__ . "/plain.txt";
$outfile = tempnam(sys_get_temp_dir(), "ssl");
$vout= $outfile . ".vout";

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
print("S/MIME attached\nPlain text:\n");
readfile($infile);
var_dump(openssl_cms_sign($infile, $outfile, openssl_x509_read($single_cert), $privkey, $headers));
var_dump(openssl_cms_verify($outfile,OPENSSL_CMS_NOVERIFY, NULL, array(), NULL, $vout));
print("\nValidated content:\n");
readfile($vout);

if (file_exists($outfile)) {
    echo "true\n";
    unlink($outfile);
}

if (file_exists($vout)) {
    echo "true\n";
    unlink($vout);
}

// test three forms of detached signatures:
// PEM first
print("\nPEM Detached:\n");
var_dump(openssl_cms_sign($infile, $outfile, openssl_x509_read($single_cert), $privkey, $headers,
	     OPENSSL_CMS_DETACHED|OPENSSL_CMS_BINARY,OPENSSL_ENCODING_PEM));
var_dump(openssl_cms_verify($infile,OPENSSL_CMS_NOVERIFY|OPENSSL_CMS_DETACHED|OPENSSL_CMS_BINARY,
         NULL, array(), NULL, $vout, NULL, $outfile, OPENSSL_ENCODING_PEM));
print("\nValidated content:\n");
readfile($vout);
if (file_exists($outfile)) {
    echo "true\n";
    unlink($outfile);
}

if (file_exists($vout)) {
    echo "true\n";
    unlink($vout);
}

// DER next
print("\nDER Detached:\n");
var_dump(openssl_cms_sign($infile, $outfile, openssl_x509_read($single_cert), $privkey, $headers,
	     OPENSSL_CMS_DETACHED|OPENSSL_CMS_BINARY,OPENSSL_ENCODING_DER));
var_dump(openssl_cms_verify($infile,OPENSSL_CMS_NOVERIFY|OPENSSL_CMS_DETACHED|OPENSSL_CMS_BINARY,
         NULL, array(), NULL, $vout, NULL, $outfile, OPENSSL_ENCODING_DER));
print("\nValidated content:\n");
readfile($vout);
// extreme measures to avoid stupid temporary errors for failure to unlink a file.
if (file_exists($outfile)) {
    echo "true\n";
    unlink($outfile);
}
$outfile=$outfile . "x";
if (file_exists($vout)) {
    echo "true\n";
    unlink($vout);
}

// S/MIME next
print("\nS/MIME Detached (an error):\n");
var_dump(openssl_cms_sign($infile, $outfile, openssl_x509_read($single_cert), $privkey, $headers,
	     OPENSSL_CMS_DETACHED,OPENSSL_ENCODING_SMIME));
var_dump(openssl_cms_verify($infile,OPENSSL_CMS_NOVERIFY|OPENSSL_CMS_DETACHED,
         NULL, array(), NULL, $vout, NULL, $outfile, OPENSSL_ENCODING_SMIME));
if (file_exists($outfile)) {
    echo "true\n";
    unlink($outfile);
}

if (file_exists($vout)) {
    echo "true\n";
    unlink($vout);
}
?>
--EXPECTF--
S/MIME attached
Plain text:
Now is the winter of our discontent.
bool(true)
bool(true)

Validated content:
Now is the winter of our discontent.
true
true

PEM Detached:
bool(true)
bool(true)

Validated content:
Now is the winter of our discontent.
true
true

DER Detached:
bool(true)
bool(true)

Validated content:
Now is the winter of our discontent.
true
true

S/MIME Detached (an error):

Warning: openssl_cms_sign(): Detached signatures not possible with S/MIME encoding in %s on line %d
bool(false)

Warning: openssl_cms_verify(): Detached signatures not possible with S/MIME encoding in %s on line %d
bool(false)
