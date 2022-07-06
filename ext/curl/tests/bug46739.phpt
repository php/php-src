--TEST--
Bug #46739 (array returned by curl_getinfo should contain content_type key)
--SKIPIF--
<?php
include 'skipif.inc';
?>
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init("{$host}/get.inc");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

curl_exec($ch);
$info = curl_getinfo($ch);

echo (array_key_exists('content_type', $info)) ? "set" : "not set";
?>
--EXPECT--
set
