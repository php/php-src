--TEST--
resource openssl_csr_get_public_key ( mixed $csr [, bool $use_shortnames = true ] ) ;
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

var_dump(openssl_csr_get_public_key($csr, true));
var_dump(openssl_csr_get_public_key($csr, false));
?>
--CLEAN--
<?php
unset($config);
unset($config_arg);
unset($dn);
unset($args);
unset($csr);
openssl_free_key($privkey);
unset($privkey);
?>
--EXPECTF--
resource(%d) of type (OpenSSL key)
resource(%d) of type (OpenSSL key)
