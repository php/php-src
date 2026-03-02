--TEST--
openssl_csr_new() attributes setting tests
--EXTENSIONS--
openssl
--FILE--
<?php

$dn = array(
    "countryName" => "UK",
    "stateOrProvinceName" => "England",
    "localityName" => "London",
    "commonName" => "test.php.net",
    "emailAddress" => "test.php@php.net"
);


$config = __DIR__ . DIRECTORY_SEPARATOR . 'openssl_csr_attribs.cnf';

$config_arg = array('config' => $config);

$args = array(
    "digest_alg" => "sha256",
    "private_key_bits" => 2048,
    "private_key_type" => OPENSSL_KEYTYPE_DSA,
    "encrypt_key" => true,
    "config" => $config,
);

$privkey = 'file://' . __DIR__ . '/private_rsa_2048.key';

$csr = openssl_csr_new(
    $dn,
    $privkey,
    $args,
    [
        'emailAddress' => 'info@example.com',
        'aansluitNummer' => '11112222',
        'postalCode' => 'N11',
    ]
);


var_dump(openssl_csr_get_subject($csr));
var_dump(openssl_csr_export($csr, $output));
var_dump($output);

var_dump(openssl_csr_new(
    $dn,
    $privkey,
    $args,
    ['wrong' => 'info@example.com']
));

?>
--EXPECTF--
array(5) {
  ["C"]=>
  string(2) "UK"
  ["ST"]=>
  string(7) "England"
  ["L"]=>
  string(6) "London"
  ["CN"]=>
  string(12) "test.php.net"
  ["emailAddress"]=>
  string(16) "test.php@php.net"
}
bool(true)
string(1269) "-----BEGIN CERTIFICATE REQUEST-----
MIIDcDCCAlgCAQAwaDELMAkGA1UEBhMCVUsxEDAOBgNVBAgMB0VuZ2xhbmQxDzAN
BgNVBAcMBkxvbmRvbjEVMBMGA1UEAwwMdGVzdC5waHAubmV0MR8wHQYJKoZIhvcN
AQkBFhB0ZXN0LnBocEBwaHAubmV0MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB
CgKCAQEArbUmVW1Y+rJzZRC3DYB0kdIgvk7MAday78ybGPPDhVlbAb4CjWbaPs4n
yUCTEt9KVG0H7pXHxDbWSsC2974zdvqlP0L2op1/M2SteTcGCBOdwGH2jORVAZL8
/WbTOf9IpKAM77oN14scsyOlQBJqhh+xrLg8ksB2dOos54yDqo0Tq7R5tldV+alK
ZXWlJnqRCfFuxvqtfWI5nGTAedVZhvjQfLQQgujfXHoFWoGbXn2buzfwKGJEeqWP
bQOZF/FeOJPlgOBhhDb3BAFNVCtM3k71Rblj54pNd3yvq152xsgFd0o3s15fuSwZ
gerUjeEuw/wTK9k7vyp+MrIQHQmPdQIDAQABoIHCMAkGA1UECTECDAAwCQYDVQQX
MQIMADAMBgNVBBExBQwDTjExMBIGA1UEFDELDAkwMTIzNDU2NzgwFQYDVQQpMQ4M
DE9yZ2FuaXNhdGlvbjAZBgsrBgEEAdgOiH4CATEKDAgxMTExMjIyMjAZBgsrBgEE
AdgOiH4CAjEKDAgxMjM0NTY3ODAaBgkqhkiG9w0BCQ4xDTALMAkGA1UdEwQCMAAw
HwYJKoZIhvcNAQkBMRIWEGluZm9AZXhhbXBsZS5jb20wDQYJKoZIhvcNAQELBQAD
ggEBAAoPI/sWY0QKPMEBuRp6MHcvWgSExwkkQfRJQZlYdepu6Tw0iZwYRTOR4sEn
Vz95qsrWqHp6QkXxdFG9FPHi4N66OX2Xb5TtHgDGMxrJTwbH+7VdsJiXLkWbeLuo
zKv8BsrhLRYiZkl+VWIrNyOcK7ao2sD+D3YkCBA4JK4OFhfhxY43D2sme7aEQVjr
S+UvEjuIALN0AP6gO2AMiUODPBrjsPI3NpN40VUvVU+Hsp1Tlqvth/AYASuGT2yt
M5YdcSm7JwaGAwIgOv8XPUQGem52yMEvzySRC4ZyTddfiZAkeTLmbh+SMVbHXXOk
UeEz+fvmQ4L+sc3RE8u+M8g31LM=
-----END CERTIFICATE REQUEST-----
"

Warning: openssl_csr_new(): attributes: wrong is not a recognized attribute name in %s on line %d
object(OpenSSLCertificateSigningRequest)#%d (0) {
}