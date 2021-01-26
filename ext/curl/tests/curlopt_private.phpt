--TEST--
CURLOPT_PRIVATE
--SKIPIF--
<?php
if (!extension_loaded('curl')) exit("skip curl extension not loaded");
?>
--FILE--
<?php
$curl = curl_init("foobar");
$obj = new stdClass;
curl_setopt($curl, CURLOPT_PRIVATE, $obj);
var_dump($obj === curl_getinfo($curl, CURLINFO_PRIVATE));

$curl2 = curl_copy_handle($curl);
var_dump($obj === curl_getinfo($curl2, CURLINFO_PRIVATE));
$obj2 = new stdClass;
curl_setopt($curl2, CURLOPT_PRIVATE, $obj2);
var_dump($obj === curl_getinfo($curl, CURLINFO_PRIVATE));
var_dump($obj2 === curl_getinfo($curl2, CURLINFO_PRIVATE));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
