--TEST--
Test curl_getinfo() function with CURLINFO_* from curl >= 7.52.0
--EXTENSIONS--
curl
--FILE--
<?php

include 'server.inc';

$ch = curl_init();
$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_exec($ch);
var_dump(CURLPROTO_HTTP === curl_getinfo($ch, CURLINFO_PROTOCOL));
var_dump(0 === curl_getinfo($ch, CURLINFO_PROXY_SSL_VERIFYRESULT));
var_dump(curl_getinfo($ch, CURLINFO_SCHEME));
curl_close($ch);
?>
--EXPECT--
bool(true)
bool(true)
string(4) "HTTP"
