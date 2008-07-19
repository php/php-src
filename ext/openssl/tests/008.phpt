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
$c = (binary) "file://" . dirname(__FILE__) . "/cert.crt";
$d = "invalid cert";
$e = "non-ascii-unicode\u7000";	// 
$f = openssl_x509_read($a);
$g = (unicode) $a;

var_dump(openssl_x509_export($a, $output));	// read cert as a binary string
var_dump(openssl_x509_export($b, $output2));	// read cert from a file, unicode string
var_dump(openssl_x509_export($c, $output3));	// read cert from a file, binary string
var_dump(openssl_x509_export($d, $output4));	// read an invalid cert, fails
var_dump(openssl_x509_export((binary)$d, $output4b));	// read an invalid cert (binary), fails
var_dump(openssl_x509_export($e, $output5));	// read an invalid unicode string, fails
var_dump(openssl_x509_export($f, $output6));	// read cert from a resource
var_dump(openssl_x509_export($g, $output7));	// read cert as a unicode string, fails (should be binary only)

$outfilename = tempnam(b"/tmp", b"ssl");
if ($outfilename === false) {
        unlink($outfile);
        die("failed to get a temporary filename!");
}
echo "---\n";

var_dump(openssl_x509_export_to_file($a, $outfilename));       // read cert as a binary string
var_dump(openssl_x509_export_to_file($b, $outfilename));      // read cert from a file, unicode string
var_dump(openssl_x509_export_to_file($c, $outfilename));      // read cert from a file, binary string
var_dump(openssl_x509_export_to_file($d, $outfilename));      // read an invalid cert, fails
var_dump(openssl_x509_export_to_file((binary) $d, $outfilename));      // read an invalid cert (binary), fails
var_dump(openssl_x509_export_to_file($e, $outfilename));      // read an invalid unicode string, fails
var_dump(openssl_x509_export_to_file($f, $outfilename));      // read cert from a resource
var_dump(openssl_x509_export_to_file($g, $outfilename));      // read cert as a unicode string, fails (should be binary only)
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
var_dump(strcmp($output, $output4b));	// different
var_dump(strcmp($output, $output5));	// different
var_dump(strcmp($output, $output6));
var_dump(strcmp($output, $output7));
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)

Warning: openssl_x509_export(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_x509_export(): cannot get cert from parameter 1 in %s on line %d
bool(false)

Warning: openssl_x509_export(): cannot get cert from parameter 1 in %s on line %d
bool(false)

Warning: openssl_x509_export(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_x509_export(): cannot get cert from parameter 1 in %s on line %d
bool(false)
bool(true)

Warning: openssl_x509_export(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_x509_export(): cannot get cert from parameter 1 in %s on line %d
bool(false)
---
bool(true)
bool(true)
bool(true)

Warning: openssl_x509_export_to_file(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_x509_export_to_file(): cannot get cert from parameter 1 in %s on line %d
bool(false)

Warning: openssl_x509_export_to_file(): cannot get cert from parameter 1 in %s on line %d
bool(false)

Warning: openssl_x509_export_to_file(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_x509_export_to_file(): cannot get cert from parameter 1 in %s on line %d
bool(false)
bool(true)

Warning: openssl_x509_export_to_file(): Binary string expected, Unicode string received in %s on line %d

Warning: openssl_x509_export_to_file(): cannot get cert from parameter 1 in %s on line %d
bool(false)
---
bool(true)
---
int(0)
int(0)
int(0)
int(%d)
int(%d)
int(%d)
int(0)
int(%d)

