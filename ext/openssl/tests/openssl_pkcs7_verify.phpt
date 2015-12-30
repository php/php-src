--TEST--
mixed openssl_pkcs7_verify ( string $filename , int $flags [, string $outfilename [, array $cainfo [, string $extracerts [, string $content ]]]] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) print "skip";
if (OPENSSL_VERSION_NUMBER < 0x10000000) die("skip Output requires OpenSSL 1.0");
?>
--FILE--
<?php
$pemFile = "san-cert.pem";
$emlFile = "test_sample_message.eml";
$caFile = "san-ca.pem";
$fileCreateFileWrite = "w";

$outFile1 = "outFile1.out";
$outFile2 = "outFile2.out";
$outFile3 = "outFile3.out";
$outFile4 = "outFile4.out";
$outFile5 = "outFile5.out";

$fileEml = __DIR__ . DIRECTORY_SEPARATOR . "{$emlFile}";
$fileCa = __DIR__ . DIRECTORY_SEPARATOR . "{$caFile}";
$extraCerts = __DIR__ . DIRECTORY_SEPARATOR . "{$pemFile}";

$outFileName1 = __DIR__ . DIRECTORY_SEPARATOR . "{$outFile1}";
$outFileName2 = __DIR__ . DIRECTORY_SEPARATOR . "{$outFile2}";
$outFileName3 = __DIR__ . DIRECTORY_SEPARATOR . "{$outFile3}";
$outFileName4 = __DIR__ . DIRECTORY_SEPARATOR . "{$outFile4}";
$outFileName5 = __DIR__ . DIRECTORY_SEPARATOR . "{$outFile5}";

fopen($outFileName1, $fileCreateFileWrite);
fopen($outFileName2, $fileCreateFileWrite);
fopen($outFileName3, $fileCreateFileWrite);
fopen($outFileName4, $fileCreateFileWrite);
fopen($outFileName5, $fileCreateFileWrite);

var_dump(openssl_pkcs7_verify($fileEml, PKCS7_TEXT));
var_dump(openssl_pkcs7_verify($fileEml, PKCS7_BINARY));
var_dump(openssl_pkcs7_verify($fileEml, PKCS7_NOINTERN));
var_dump(openssl_pkcs7_verify($fileEml, PKCS7_NOVERIFY));
var_dump(openssl_pkcs7_verify($fileEml, PKCS7_NOCHAIN));
var_dump(openssl_pkcs7_verify($fileEml, PKCS7_NOCERTS));
var_dump(openssl_pkcs7_verify($fileEml, PKCS7_NOATTR));
var_dump(openssl_pkcs7_verify($fileEml, PKCS7_DETACHED));
var_dump(openssl_pkcs7_verify($fileEml, PKCS7_NOSIGS));

openssl_pkcs7_verify($fileEml, PKCS7_TEXT, $outFileName1);
var_dump(file_get_contents($outFileName1));
openssl_pkcs7_verify($fileEml, PKCS7_TEXT, $outFileName2, null);
var_dump(file_get_contents($outFileName2));
openssl_pkcs7_verify($fileEml, PKCS7_TEXT, $outFileName3, array());
var_dump(file_get_contents($outFileName3));
openssl_pkcs7_verify($fileEml, PKCS7_TEXT, $outFileName4, array($fileCa));
var_dump(file_get_contents($outFileName4));
openssl_pkcs7_verify($fileEml, PKCS7_TEXT, $outFileName5, array($fileCa), $extraCerts);
var_dump(file_get_contents($outFileName5));

openssl_pkcs7_verify($fileEml, PKCS7_BINARY, $outFileName1);
var_dump(file_get_contents($outFileName1));
openssl_pkcs7_verify($fileEml, PKCS7_BINARY, $outFileName2, null);
var_dump(file_get_contents($outFileName2));
openssl_pkcs7_verify($fileEml, PKCS7_BINARY, $outFileName3, array());
var_dump(file_get_contents($outFileName3));
openssl_pkcs7_verify($fileEml, PKCS7_BINARY, $outFileName4, array($fileCa));
var_dump(file_get_contents($outFileName4));
openssl_pkcs7_verify($fileEml, PKCS7_BINARY, $outFileName5, array($fileCa), $extraCerts);
var_dump(file_get_contents($outFileName5));

openssl_pkcs7_verify($fileEml, PKCS7_NOINTERN, $outFileName1);
var_dump(file_get_contents($outFileName1));
openssl_pkcs7_verify($fileEml, PKCS7_NOINTERN, $outFileName2, null);
var_dump(file_get_contents($outFileName2));
openssl_pkcs7_verify($fileEml, PKCS7_NOINTERN, $outFileName3, array());
var_dump(file_get_contents($outFileName3));
openssl_pkcs7_verify($fileEml, PKCS7_NOINTERN, $outFileName4, array($fileCa));
var_dump(file_get_contents($outFileName4));
openssl_pkcs7_verify($fileEml, PKCS7_NOINTERN, $outFileName5, array($fileCa), $extraCerts);
var_dump(file_get_contents($outFileName5));

openssl_pkcs7_verify($fileEml, PKCS7_NOVERIFY, $outFileName1);
var_dump(file_get_contents($outFileName1));
openssl_pkcs7_verify($fileEml, PKCS7_NOVERIFY, $outFileName2, null);
var_dump(file_get_contents($outFileName2));
openssl_pkcs7_verify($fileEml, PKCS7_NOVERIFY, $outFileName3, array());
var_dump(file_get_contents($outFileName3));
openssl_pkcs7_verify($fileEml, PKCS7_NOVERIFY, $outFileName4, array($fileCa));
var_dump(file_get_contents($outFileName4));
openssl_pkcs7_verify($fileEml, PKCS7_NOVERIFY, $outFileName5, array($fileCa), $extraCerts);
var_dump(file_get_contents($outFileName5));

openssl_pkcs7_verify($fileEml, PKCS7_NOCHAIN, $outFileName1);
var_dump(file_get_contents($outFileName1));
openssl_pkcs7_verify($fileEml, PKCS7_NOCHAIN, $outFileName2, null);
var_dump(file_get_contents($outFileName2));
openssl_pkcs7_verify($fileEml, PKCS7_NOCHAIN, $outFileName3, array());
var_dump(file_get_contents($outFileName3));
openssl_pkcs7_verify($fileEml, PKCS7_NOCHAIN, $outFileName4, array($fileCa));
var_dump(file_get_contents($outFileName4));
openssl_pkcs7_verify($fileEml, PKCS7_NOCHAIN, $outFileName5, array($fileCa), $extraCerts);
var_dump(file_get_contents($outFileName5));

openssl_pkcs7_verify($fileEml, PKCS7_NOCERTS, $outFileName1);
var_dump(file_get_contents($outFileName1));
openssl_pkcs7_verify($fileEml, PKCS7_NOCERTS, $outFileName2, null);
var_dump(file_get_contents($outFileName2));
openssl_pkcs7_verify($fileEml, PKCS7_NOCERTS, $outFileName3, array());
var_dump(file_get_contents($outFileName3));
openssl_pkcs7_verify($fileEml, PKCS7_NOCERTS, $outFileName4, array($fileCa));
var_dump(file_get_contents($outFileName4));
openssl_pkcs7_verify($fileEml, PKCS7_NOCERTS, $outFileName5, array($fileCa), $extraCerts);
var_dump(file_get_contents($outFileName5));

openssl_pkcs7_verify($fileEml, PKCS7_NOATTR, $outFileName1);
var_dump(file_get_contents($outFileName1));
openssl_pkcs7_verify($fileEml, PKCS7_NOATTR, $outFileName2, null);
var_dump(file_get_contents($outFileName2));
openssl_pkcs7_verify($fileEml, PKCS7_NOATTR, $outFileName3, array());
var_dump(file_get_contents($outFileName3));
openssl_pkcs7_verify($fileEml, PKCS7_NOATTR, $outFileName4, array($fileCa));
var_dump(file_get_contents($outFileName4));
openssl_pkcs7_verify($fileEml, PKCS7_NOATTR, $outFileName5, array($fileCa), $extraCerts);
var_dump(file_get_contents($outFileName5));

openssl_pkcs7_verify($fileEml, PKCS7_DETACHED, $outFileName1);
var_dump(file_get_contents($outFileName1));
openssl_pkcs7_verify($fileEml, PKCS7_DETACHED, $outFileName2, null);
var_dump(file_get_contents($outFileName2));
openssl_pkcs7_verify($fileEml, PKCS7_DETACHED, $outFileName3, array());
var_dump(file_get_contents($outFileName3));
openssl_pkcs7_verify($fileEml, PKCS7_DETACHED, $outFileName4, array($fileCa));
var_dump(file_get_contents($outFileName4));
openssl_pkcs7_verify($fileEml, PKCS7_DETACHED, $outFileName5, array($fileCa), $extraCerts);
var_dump(file_get_contents($outFileName5));

openssl_pkcs7_verify($fileEml, PKCS7_NOSIGS, $outFileName1);
var_dump(file_get_contents($outFileName1));
openssl_pkcs7_verify($fileEml, PKCS7_NOSIGS, $outFileName2, null);
var_dump(file_get_contents($outFileName2));
openssl_pkcs7_verify($fileEml, PKCS7_NOSIGS, $outFileName3, array());
var_dump(file_get_contents($outFileName3));
openssl_pkcs7_verify($fileEml, PKCS7_NOSIGS, $outFileName4, array($fileCa));
var_dump(file_get_contents($outFileName4));
openssl_pkcs7_verify($fileEml, PKCS7_NOSIGS, $outFileName5, array($fileCa), $extraCerts);
var_dump(file_get_contents($outFileName5));

?>
--CLEAN--
<?php
unset($pemFile);
unset($emlFile);
unset($caFile);
unset($outFile1);
unset($outFile2);
unset($outFile3);
unset($outFile4);
unset($outFile5);
unset($fileEml);
unset($fileCa);
unset($extraCerts);
unlink($outFileName1);
unlink($outFileName2);
unlink($outFileName3);
unlink($outFileName4);
unlink($outFileName5);
unset($outFileName1);
unset($outFileName2);
unset($outFileName3);
unset($outFileName4);
unset($outFileName5);
?>
--EXPECTF--
int(%i)
int(%i)
int(%i)
int(%i)
int(%i)
int(%i)
int(%i)
int(%i)
int(%i)
string(%d) ""

Warning: openssl_pkcs7_verify() expects parameter %d to be array, null given in %s on line %d
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""

Warning: openssl_pkcs7_verify() expects parameter %d to be array, null given in %s on line %d
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""

Warning: openssl_pkcs7_verify() expects parameter %d to be array, null given in %s on line %d
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""

Warning: openssl_pkcs7_verify() expects parameter %d to be array, null given in %s on line %d
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""

Warning: openssl_pkcs7_verify() expects parameter %d to be array, null given in %s on line %d
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""

Warning: openssl_pkcs7_verify() expects parameter %d to be array, null given in %s on line %d
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""

Warning: openssl_pkcs7_verify() expects parameter %d to be array, null given in %s on line %d
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""

Warning: openssl_pkcs7_verify() expects parameter %d to be array, null given in %s on line %d
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""

Warning: openssl_pkcs7_verify() expects parameter %d to be array, null given in %s on line %d
string(%d) ""
string(%d) ""
string(%d) ""
string(%d) ""
