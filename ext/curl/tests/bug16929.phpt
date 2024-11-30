--TEST--
Bug #16929 (curl_getinfo($ch, CURLINFO_CONTENT_TYPE) returns false when Content-Type header is not set)
--EXTENSIONS--
curl
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init();
$url = "{$host}/get.inc?test=emptycontenttype";

curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, $url);
curl_exec($ch);

$ct = curl_getinfo($ch, CURLINFO_CONTENT_TYPE);

var_dump($ct);

$info = curl_getinfo($ch)['content_type'];

var_dump($info);

curl_close($ch);


curl_close($ch);
?>
--EXPECTF--
bool(false)
bool(false)
