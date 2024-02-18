--TEST--
Test CurlHandle::getInfo() function with CURLINFO_* from curl >= 7.52.0
--EXTENSIONS--
curl
--FILE--
<?php

include 'server.inc';

$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
$ch = new CurlHandle($url);
$ch->exec();
var_dump(CURLPROTO_HTTP === $ch->getInfo(CURLINFO_PROTOCOL));
var_dump(0 === $ch->getInfo(CURLINFO_PROXY_SSL_VERIFYRESULT));
var_dump($ch->getInfo(CURLINFO_SCHEME));
--EXPECT--
bool(true)
bool(true)
string(4) "HTTP"
