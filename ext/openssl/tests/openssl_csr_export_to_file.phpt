--TEST--
bool openssl_csr_export_to_file ( resource $csr , string $outfilename [, bool $notext = true ] ) ;
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) print "skip";
?>
--FILE--
<?php
$config = __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf';
$config_arg = array('config' => $config);
$crs_exported_file_notext_true = __DIR__ . DIRECTORY_SEPARATOR . "crs-exported-file-notext-true.crs";
$crs_exported_file_notext_false = __DIR__ . DIRECTORY_SEPARATOR . "crs-exported-file-notext-false.crs";

$dn = array(
    "countryName" => "BR",
    "stateOrProvinceName" => "Rio Grande do Sul",
    "localityName" => "Porto Alegre",
    "commonName" => "Marcosptf",
    "emailAddress" => "marcosptf@yahoo.com.br"
);

$args = array(
    "digest_alg" => "sha1",
    "private_key_bits" => 2048,
    "private_key_type" => OPENSSL_KEYTYPE_DSA,
    "encrypt_key" => true,
    "config" => $config
);

$privkey = openssl_pkey_new($config_arg);
$csr = openssl_csr_new($dn, $privkey, $args);

var_dump(openssl_csr_export_to_file($csr, $crs_exported_file_notext_true, true));
var_dump(openssl_csr_export_to_file($csr, $crs_exported_file_notext_false, false));
var_dump(file_exists($crs_exported_file_notext_true));
var_dump(file_exists($crs_exported_file_notext_false));
?>
--CLEAN--
<?php 
unset($config);
unset($config_arg);
unset($crs_exported_file_notext_true);
unset($crs_exported_file_notext_false);
unset($dn);
unset($args);
unset($csr);
openssl_free_key($privkey);
unset($privkey);
unlink(__DIR__ . DIRECTORY_SEPARATOR . "crs-exported-file-notext-true.crs");
unlink(__DIR__ . DIRECTORY_SEPARATOR . "crs-exported-file-notext-false.crs"); 
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
