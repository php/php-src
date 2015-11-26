--TEST--
Bug #36732 (add support for req_extensions in openss_csr_new and sign)
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip");
if (OPENSSL_VERSION_NUMBER < 0x009070af) die("skip");
?>
--FILE--
<?php 
$configargs = array(
        "req_extensions" => "v3_req",
        "x509_extensions" => "usr_cert",
		"config" => __DIR__."/openssl.cnf",
);

$dn = array(
        "countryName" => "GB",
        "stateOrProvinceName" => "Berkshire",
        "localityName" => "Newbury",
        "organizationName" => "My Company Ltd",
        "commonName" => "Demo Cert"
);

$key = openssl_pkey_new();
$csr = openssl_csr_new($dn, $key, $configargs);
$crt = openssl_csr_sign($csr, NULL, $key, 365, $configargs);

$str = '';
openssl_csr_export($csr, $str, false);

if (strpos($str, 'Requested Extensions:')) {
	echo "Ok\n";
}
openssl_x509_export($crt, $str, false);
if (strpos($str, 'X509v3 extensions:')) {
	echo "Ok\n";
}
?>
--EXPECTF--
Ok
Ok
