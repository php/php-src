--TEST--
GH-16357 (openssl may modify member types of certificate arrays)
--EXTENSIONS--
openssl
--FILE--
<?php
$infile = __DIR__ . "/cert.crt";
$outfile = __DIR__ . "/gh16357.txt";
$certs = [123];
var_dump(openssl_pkcs7_encrypt($infile, $outfile, $certs, null));
var_dump($certs);
?>
--CLEAN--
<?php
unlink(__DIR__ . "/gh16357.txt");
?>
--EXPECT--
bool(false)
array(1) {
  [0]=>
  int(123)
}
