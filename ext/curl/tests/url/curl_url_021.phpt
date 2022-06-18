--TEST--
CurlUrl::setHost() and CurlUrl::getHost() with bad hostname
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x075100) die('skip requires curl >= 7.81.0');
?>
--FILE--
<?php

$url = new CurlUrl();
$url->setHost('www.example.com');
var_dump($url->getHost());

try {
    $url->setHost('www.ex ample.com');
} catch (CurlUrlException $e) {
    var_dump($e->getCode() == CurlUrlException::BAD_HOSTNAME);
}
var_dump($url->getHost());
?>
--EXPECT--
string(15) "www.example.com"
bool(true)
string(15) "www.example.com"
