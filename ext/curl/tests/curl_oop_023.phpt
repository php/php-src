--TEST--
Test CurlHandle::getInfo() function with CURLINFO_HTTP_VERSION parameter
--EXTENSIONS--
curl
--FILE--
<?php

include 'server.inc';

$ch = new CurlHandle;
var_dump(0 === $ch->getInfo(CURLINFO_HTTP_VERSION));

$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
$ch->setOpt(CURLOPT_URL, $url)->exec();
var_dump(CURL_HTTP_VERSION_1_1 === curl_getinfo($ch, CURLINFO_HTTP_VERSION));
curl_close($ch);
--EXPECT--
bool(true)
bool(true)
