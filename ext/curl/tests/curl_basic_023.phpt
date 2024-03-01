--TEST--
Test curl_getinfo() function with CURLINFO_HTTP_VERSION parameter
--EXTENSIONS--
curl
--FILE--
<?php

include 'server.inc';

$ch = curl_init();
var_dump(0 === curl_getinfo($ch, CURLINFO_HTTP_VERSION));

$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_exec($ch);
var_dump(CURL_HTTP_VERSION_1_1 === curl_getinfo($ch, CURLINFO_HTTP_VERSION));
curl_close($ch);
?>
--EXPECT--
bool(true)
bool(true)
