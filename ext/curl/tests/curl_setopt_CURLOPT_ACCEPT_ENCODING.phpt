--TEST--
Test curl_setopt() with CURLOPT_ACCEPT_ENCODING
--EXTENSIONS--
curl
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();

$ch = curl_init();

$url = "{$host}/get.inc?test=";
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_ACCEPT_ENCODING, "gzip");
curl_setopt($ch, CURLINFO_HEADER_OUT, 1);

// First execution, with gzip accept
curl_exec($ch);
echo curl_getinfo($ch, CURLINFO_HEADER_OUT);

// Second execution, with the encoding accept disabled
curl_setopt($ch, CURLOPT_ACCEPT_ENCODING, NULL);
curl_exec($ch);
echo curl_getinfo($ch, CURLINFO_HEADER_OUT);

curl_close($ch);
?>
--EXPECTF--
GET /get.inc?test= HTTP/1.1
Host: %s
Accept: */*
Accept-Encoding: gzip

GET /get.inc?test= HTTP/1.1
Host: %s
Accept: */*
