--TEST--
curl_getinfo - CURLINFO_CERTINFO
--EXTENSIONS--
curl
--SKIPIF--
<?php
include 'skipif-nocaddy.inc';
?>
--FILE--
<?php

$ch = curl_init("https://localhost/ping");
curl_setopt($ch, CURLOPT_CERTINFO, 1);
curl_exec($ch);
echo "\n";

$certinfo = curl_getinfo($ch, CURLINFO_CERTINFO);

var_dump(empty($certinfo));
var_dump(str_starts_with($certinfo[0]['Cert'], '-----BEGIN CERTIFICATE-----'));

?>
--EXPECT--
pong
bool(false)
bool(true)
