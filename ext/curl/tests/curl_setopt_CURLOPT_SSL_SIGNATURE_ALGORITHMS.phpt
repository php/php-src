--TEST--
Curl option CURLOPT_SSL_SIGNATURE_ALGORITHMS
--EXTENSIONS--
curl
--SKIPIF--
<?php
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x080e00) die("skip: test works only with curl >= 8.14.0");

include 'skipif-nocaddy.inc';
?>
--FILE--
<?php

$ch = curl_init('https://localhost/ping');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);

var_dump(curl_exec($ch));

var_dump(curl_setopt($ch, CURLOPT_SSL_SIGNATURE_ALGORITHMS, 'invalid-value'));
var_dump(curl_exec($ch));
var_dump(curl_error($ch));

var_dump(curl_setopt($ch, CURLOPT_SSL_SIGNATURE_ALGORITHMS, 'ECDSA+SHA256:RSA+SHA256:DSA+SHA256:ed25519'));
var_dump(curl_exec($ch));

var_dump(curl_setopt($ch, CURLOPT_SSL_SIGNATURE_ALGORITHMS, null));
var_dump(curl_exec($ch));

?>
--EXPECT--
string(4) "pong"
bool(true)
bool(false)
string(52) "failed setting signature algorithms: 'invalid-value'"
bool(true)
string(4) "pong"
bool(true)
string(4) "pong"
