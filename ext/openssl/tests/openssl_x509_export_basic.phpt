--TEST--
openssl_x509_export() tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$cert_file = __DIR__ . "/cert.crt";

$a = file_get_contents($cert_file);
$b = "file://" . $cert_file;
$c = "invalid cert";
$d = openssl_x509_read($a);
$e = array();

var_dump(openssl_x509_export($a, $output));  // read cert as a binary string
var_dump(openssl_x509_export($b, $output2)); // read cert from a filename string
var_dump(openssl_x509_export($c, $output3)); // read an invalid cert, fails
var_dump(openssl_x509_export($d, $output4)); // read cert from a resource

try {
    openssl_x509_export($e, $output5); // read an array, fails
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

if (PHP_EOL !== "\n") {
    $a = str_replace(PHP_EOL, "\n", $a);
}

var_dump(strcmp($output, $a));
var_dump(strcmp($output, $output2));
var_dump(strcmp($output, $output3));
var_dump(strcmp($output, $output4)); // different
var_dump(strcmp($output, $output5)); // different
?>
--EXPECTF--
bool(true)
bool(true)

Warning: openssl_x509_export(): X.509 Certificate cannot be retrieved in %s on line %d
bool(false)
bool(true)
openssl_x509_export(): Argument #1 ($certificate) must be of type OpenSSLCertificate|string, array given
int(0)
int(0)
int(%d)
int(0)
int(%d)
