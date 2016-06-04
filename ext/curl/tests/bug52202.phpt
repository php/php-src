--TEST--
Bug #52202 (CURLOPT_PRIVATE gets clobbered)
--SKIPIF--
<?php
if (!extension_loaded('curl')) exit("skip curl extension not loaded");
?>
--FILE--
<?php
$curl = curl_init("http://www.google.com");
curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
curl_setopt($curl, CURLOPT_PRIVATE, "123");
curl_setopt($curl, CURLOPT_CONNECTTIMEOUT, 1);
curl_setopt($curl, CURLOPT_TIMEOUT, 1);
curl_exec($curl);

var_dump(curl_getinfo($curl, CURLINFO_PRIVATE));
--EXPECT--
string(3) "123"
