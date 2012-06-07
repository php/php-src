--TEST--
Bug #54995 (Missing CURLINFO_RESPONSE_CODE support)
--SKIPIF--
<?php 
if (!extension_loaded("curl")) {
	exit("skip curl extension not loaded");
}
if ($curl_version['version_number'] > 0x070a08) {
	exit("skip: tests works a versions of curl >= 7.10.8");
}
if (false === getenv('PHP_CURL_HTTP_REMOTE_SERVER'))  {
	exit("skip PHP_CURL_HTTP_REMOTE_SERVER env variable is not defined");
}
?>
--FILE--
<?php

$host = getenv('PHP_CURL_HTTP_REMOTE_SERVER');
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.php");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

var_dump(curl_getinfo($ch, CURLINFO_HTTP_CODE) == curl_getinfo($ch, CURLINFO_RESPONSE_CODE));

curl_exec($ch);
curl_close($ch);

?>
--EXPECTF--
bool(true)
