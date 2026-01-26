--TEST--
GH-21023 (crash with CURLOPT_XFERINFOFUNCTION set with an invalid callback)
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();
$url = "{$host}/get.inc";
$ch = curl_init($url);
curl_setopt($ch, CURLOPT_NOPROGRESS, 0);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_XFERINFOFUNCTION, null);
curl_exec($ch);
$ch = curl_init($url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_PROGRESSFUNCTION, null);
curl_exec($ch);
$ch = curl_init($url);
curl_setopt($ch, CURLOPT_WILDCARDMATCH, 1);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_FNMATCH_FUNCTION, null);
curl_exec($ch);
echo "OK", PHP_EOL;
?>
--EXPECT--
OK
