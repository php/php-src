--TEST--
Bug #66109 (Option CURLOPT_CUSTOMREQUEST can't be reset to default.)
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
	exit("skip curl extension not loaded");
}
if (false === getenv('PHP_CURL_HTTP_REMOTE_SERVER'))  {
	exit("skip PHP_CURL_HTTP_REMOTE_SERVER env variable is not defined");
}
?>
--FILE--
<?php

$host = getenv('PHP_CURL_HTTP_REMOTE_SERVER');
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.php?test=method");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

curl_setopt($ch, CURLOPT_CUSTOMREQUEST, 'DELETE');
var_dump(curl_exec($ch));

curl_setopt($ch, CURLOPT_CUSTOMREQUEST, NULL);
var_dump(curl_exec($ch));

curl_close($ch);

?>
--EXPECTF--
string(6) "DELETE"
string(3) "GET"
