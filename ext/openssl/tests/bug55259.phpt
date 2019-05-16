--TEST--
Bug #55259 (openssl extension does not get the DH parameters from DH key resource)
--SKIPIF--
<?php if (!extension_loaded("openssl")) die("skip"); ?>
--FILE--
<?php

$phex = 'dcf93a0b883972ec0e19989ac5a2ce310e1d37717e8d9571bb7623731866e61e' .
        'f75a2e27898b057f9891c2e27a639c3f29b60814581cd3b2ca3986d268370557' .
        '7d45c2e7e52dc81c7a171876e5cea74b1448bfdfaf18828efd2519f14e45e382' .
        '6634af1949e5b535cc829a483b8a76223e5d490a257f05bdff16f2fb22c583ab';
$dh_details = array('p' => $phex, 'g' => '2');
$dh = openssl_pkey_new(array('dh'=> array('p' => $phex, 'g' => '2')));
var_dump($dh);
$dh = openssl_pkey_new(array('dh'=> array( 'p' => hex2bin($phex), 'g' => '2')));
$details = openssl_pkey_get_details($dh);
var_dump(bin2hex($details['dh']['p']));
var_dump($details['dh']['g']);
var_dump(strlen($details['dh']['pub_key']) > 0);
var_dump(strlen($details['dh']['priv_key']) > 0);
echo "Done";
?>
--EXPECT--
bool(false)
string(256) "dcf93a0b883972ec0e19989ac5a2ce310e1d37717e8d9571bb7623731866e61ef75a2e27898b057f9891c2e27a639c3f29b60814581cd3b2ca3986d2683705577d45c2e7e52dc81c7a171876e5cea74b1448bfdfaf18828efd2519f14e45e3826634af1949e5b535cc829a483b8a76223e5d490a257f05bdff16f2fb22c583ab"
string(1) "2"
bool(true)
bool(true)
Done
