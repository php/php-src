--TEST--
openssl_csr_get_subject() tests
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
?>
--FILE--
<?php
$config = __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf';
$phex = 'dcf93a0b883972ec0e19989ac5a2ce310e1d37717e8d9571bb7623731866e61e' .
        'f75a2e27898b057f9891c2e27a639c3f29b60814581cd3b2ca3986d268370557' .
        '7d45c2e7e52dc81c7a171876e5cea74b1448bfdfaf18828efd2519f14e45e382' .
        '6634af1949e5b535cc829a483b8a76223e5d490a257f05bdff16f2fb22c583ab';
$dh_details = array('p' => $phex, 'g' => '2');
$dh = openssl_pkey_new(array(
	'dh'=> array('p' => hex2bin($phex), 'g' => '2'))
);

$dn = array(
    "countryName" => "BR",
    "stateOrProvinceName" => "Rio Grande do Sul",
    "localityName" => "Porto Alegre",
    "commonName" => "Henrique do N. Angelo",
    "emailAddress" => "hnangelo@php.net"
);

$args = array(
    "digest_alg" => "sha1",
    "private_key_bits" => 2048,
    "private_key_type" => OPENSSL_KEYTYPE_DSA,
    "encrypt_key" => true,
    "config" => $config,
);

$privkey_file = 'file://' . dirname(__FILE__) . '/private_rsa_2048.key';
$csr = openssl_csr_new($dn, $privkey_file, $args);
$csr_file = file_get_contents(dirname(__FILE__) . '/cert.csr');

var_dump(openssl_csr_get_subject($csr_file));
var_dump(openssl_csr_get_subject($csr, false));
?>
--EXPECTF--
array(6) {
  ["C"]=>
  string(2) "NL"
  ["ST"]=>
  string(13) "Noord Brabant"
  ["L"]=>
  string(4) "Uden"
  ["O"]=>
  string(10) "Triconnect"
  ["OU"]=>
  string(10) "Triconnect"
  ["CN"]=>
  string(15) "*.triconnect.nl"
}
array(6) {
  ["countryName"]=>
  string(2) "BR"
  ["stateOrProvinceName"]=>
  string(17) "Rio Grande do Sul"
  ["localityName"]=>
  string(12) "Porto Alegre"
  ["commonName"]=>
  string(21) "Henrique do N. Angelo"
  ["emailAddress"]=>
  string(16) "hnangelo@php.net"
  ["organizationName"]=>
  string(24) "Internet Widgits Pty Ltd"
}
