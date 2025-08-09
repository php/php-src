--TEST--
GH-19369: openssl_sign with alias algorithms
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!in_array('sha256WithRSAEncryption', openssl_get_md_methods(true))) {
    die('skip sha256WithRSAEncryption alias not present');
}
?>
--FILE--
<?php
$digests             = openssl_get_md_methods();
$digests_and_aliases = openssl_get_md_methods(true);
$digest_aliases      = array_diff($digests_and_aliases, $digests);

$data = "Testing openssl_sign() with alias algorithm";
$privkey = "file://" . __DIR__ . "/private_rsa_1024.key";

var_dump(openssl_sign($data, $sign, $privkey, 'sha256WithRSAEncryption'));

?>
--EXPECT--
bool(true)
