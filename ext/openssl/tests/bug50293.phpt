--TEST--
Bug #50293 (Several openssl functions ignore the VCWD)
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
?>
--FILE--
<?php
$cert = "file://" . __DIR__ . "/cert.crt";
$priv = "file://" . __DIR__ . "/private_rsa_1024.key";
$config = __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf';

$dn = [
    "countryName" => "GB",
    "stateOrProvinceName" => "Berkshire",
    "localityName" => "Newbury",
    "organizationName" => "My Company Ltd",
    "commonName" => "Demo Cert",
];

$args = array(
    "digest_alg" => "sha256",
    "private_key_bits" => 2048,
    "private_key_type" => OPENSSL_KEYTYPE_RSA,
    "encrypt_key" => true,
    "config" => $config
);

mkdir(__DIR__ . "/bug50293");
chdir(__DIR__ . "/bug50293");

$privkey = openssl_pkey_get_private('file://' . __DIR__ . '/private_ec.key');
$csr = openssl_csr_new($dn, $privkey, $args);
$sscert = openssl_csr_sign($csr, null, $privkey, 365, $args);
openssl_csr_export($csr, $csrout);;
openssl_x509_export($sscert, $certout);
openssl_x509_export_to_file($sscert , "bug50293.crt", false);
openssl_pkey_export_to_file($privkey, "bug50293.pem", null, $args);

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
