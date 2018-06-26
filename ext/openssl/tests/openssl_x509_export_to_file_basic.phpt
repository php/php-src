--TEST--
openssl_x509_export_to_file() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$outfilename = dirname(__FILE__) . "/openssl_x509_export_to_file__outfilename.tmp";
$cert_file = dirname(__FILE__) . "/cert.crt";

$a = file_get_contents($cert_file);
$b = "file://" . $cert_file;
$c = "invalid cert";
$d = openssl_x509_read($a);
$e = array();

var_dump(openssl_x509_export_to_file($a, $outfilename)); // read cert as a binary string
var_dump(openssl_x509_export_to_file($b, $outfilename)); // read cert from a filename string
var_dump(openssl_x509_export_to_file($c, $outfilename)); // read an invalid cert, fails
var_dump(openssl_x509_export_to_file($d, $outfilename)); // read cert from a resource
var_dump(openssl_x509_export_to_file($e, $outfilename)); // read an array, fails
echo "---\n";
var_dump($exists = file_exists($outfilename));
?>
--CLEAN--
<?php
$outfilename = dirname(__FILE__) . "/openssl_x509_export_to_file__outfilename.tmp";
if (file_exists($outfilename)) {
    unlink($outfilename);
}
?>
--EXPECTF--
bool(true)
bool(true)

Warning: openssl_x509_export_to_file(): cannot get cert from parameter 1 in %s on line %d
bool(false)
bool(true)

Warning: openssl_x509_export_to_file(): cannot get cert from parameter 1 in %s on line %d
bool(false)
---
bool(true)
