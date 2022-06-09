--TEST--
Bug #50293 (Several openssl functions ignore the VCWD)
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
?>
--FILE--
<?php
$ssl_configargs = [
    "digest_alg" => "sha1",
    "encrypt_key" => false,
    "basicConstraints" => "CA:true",
    "keyUsage" => "cRLSign, keyCertSign",
    "nsCertType" => "sslCA, emailCA",
    "config" => __DIR__ . "/openssl.cnf",
];
$dn = [
    "countryName" => "GB",
    "stateOrProvinceName" => "Berkshire",
    "localityName" => "Newbury",
    "organizationName" => "My Company Ltd",
    "commonName" => "Demo Cert",
];
$numberofdays = '365';

mkdir(__DIR__ . "/bug50293");
chdir(__DIR__ . "/bug50293");

$privkey = openssl_pkey_new($ssl_configargs);
$csr = openssl_csr_new($dn, $privkey, $ssl_configargs);
$sscert = openssl_csr_sign($csr, null, $privkey, $numberofdays);
openssl_csr_export($csr, $csrout);
openssl_x509_export($sscert, $certout);
openssl_x509_export_to_file($sscert , "bug50293.crt", false);
openssl_pkey_export($privkey, $pkeyout);
openssl_pkey_export_to_file($privkey, "bug50293.pem");

var_dump(
    file_exists("bug50293.crt"),
    file_exists("bug50293.pem")
);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bug50293/bug50293.crt");
@unlink(__DIR__ . "/bug50293/bug50293.pem");
@rmdir(__DIR__ . "/bug50293");
?>
--EXPECT--
bool(true)
bool(true)
