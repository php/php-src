--TEST--
CurlUrl::setHost() and CurlUrl::getHost()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl();

var_dump($url->getHost());

$url->setHost('www.example.com');
var_dump($url->getHost());

$url->setHost(null);
var_dump($url->getHost());

?>
--EXPECT--
NULL
string(15) "www.example.com"
NULL
