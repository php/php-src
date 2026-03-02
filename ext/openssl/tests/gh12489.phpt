--TEST--
GH-12489: Missing sigbio creation checking in openssl_cms_verify
--EXTENSIONS--
openssl
--FILE--
<?php
$infile = __DIR__ . "/plain.txt";
$outfile = __DIR__ . "/out.cms";;
$vout = $outfile . '.vout';

$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";
$single_cert = "file://" . __DIR__ . "/cert.crt";
$assoc_headers = array("To" => "test@test", "Subject" => "testing openssl_cms_sign()");
$headers = array("test@test", "testing openssl_cms_sign()");

var_dump(openssl_cms_sign($infile, $outfile, openssl_x509_read($single_cert), $privkey, $headers,
             OPENSSL_CMS_DETACHED|OPENSSL_CMS_BINARY,OPENSSL_ENCODING_PEM));
ini_set('open_basedir', __DIR__);
var_dump(openssl_cms_verify($infile,OPENSSL_CMS_NOVERIFY|OPENSSL_CMS_DETACHED|OPENSSL_CMS_BINARY,
         NULL, array(), NULL, $vout, NULL, "../test.cms", OPENSSL_ENCODING_PEM));
var_dump(openssl_error_string());
?>
--CLEAN--
<?php
$outfile = __DIR__ . "/out.cms";;
$vout = $outfile . '.vout';

@unlink($outfile);
@unlink($vout);
?>
--EXPECTF--
bool(true)

Warning: openssl_cms_verify(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (%s) in %s on line %d
bool(false)
bool(false)
