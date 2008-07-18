--TEST--
openssl_x509_export() and openssl_x509_export_to_file() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$fp = fopen(dirname(__FILE__) . "/cert.crt","r");
$a = fread($fp,8192);
fclose($fp); 

$b = "file://" . dirname(__FILE__) . "/cert.crt";
$c = "invalid cert";
$d = openssl_x509_read($a);
$e = array();

var_dump(openssl_x509_export($a, $output));	// read cert as a binary string
var_dump(openssl_x509_export($b, $output2));	// read cert from a filename string
var_dump(openssl_x509_export($c, $output3));	// read an invalid cert, fails
var_dump(openssl_x509_export($d, $output4));	// read cert from a resource
var_dump(openssl_x509_export($e, $output5));	// read an array, fails

$outfilename = tempnam("/tmp", "ssl");
if ($outfilename === false)
        die("failed to get a temporary filename!");

echo "---\n";

var_dump(openssl_x509_export_to_file($a, $outfilename));      // read cert as a binary string
var_dump(openssl_x509_export_to_file($b, $outfilename));      // read cert from a filename string
var_dump(openssl_x509_export_to_file($c, $outfilename));      // read an invalid cert, fails
var_dump(openssl_x509_export_to_file($d, $outfilename));      // read cert from a resource
var_dump(openssl_x509_export_to_file($e, $outfilename));      // read an array, fails
echo "---\n";

var_dump($exists = file_exists($outfilename));
if ($exists) {
        @unlink($outfilename);
}
echo "---\n";

var_dump(strcmp($output, $a));
var_dump(strcmp($output, $output2));
var_dump(strcmp($output, $output3));
var_dump(strcmp($output, $output4));	// different
var_dump(strcmp($output, $output5));	// different
?>
--EXPECTF--
bool(true)
bool(true)

Warning: openssl_x509_export(): cannot get cert from parameter 1 in %s on line %d
bool(false)
bool(true)

Warning: openssl_x509_export(): cannot get cert from parameter 1 in %s on line %d
bool(false)
---
bool(true)
bool(true)

Warning: openssl_x509_export_to_file(): cannot get cert from parameter 1 in %s on line %d
bool(false)
bool(true)

Warning: openssl_x509_export_to_file(): cannot get cert from parameter 1 in %s on line %d
bool(false)
---
bool(true)
---
int(0)
int(0)
int(%d)
int(0)
int(%d)
