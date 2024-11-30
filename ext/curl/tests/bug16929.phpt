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

# Validate getinfo with empty Content-Type
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, $url);
curl_exec($ch);

$ct = curl_getinfo($ch, CURLINFO_CONTENT_TYPE);

var_dump($ct);

$info = curl_getinfo($ch)['content_type'];

var_dump($info);
curl_close($ch);

# Validate getinfo with non-empty Content-Type
$ch = curl_init();
$url = "{$host}/get.inc";

curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, $url);
curl_exec($ch);

var_dump(curl_getinfo($ch, CURLINFO_CONTENT_TYPE));
var_dump(curl_getinfo($ch)['content_type']);

curl_close($ch);
?>
--EXPECTF--
NULL
NULL
string(%d) "text/html; charset=UTF-8"
string(%d) "text/html; charset=UTF-8"
