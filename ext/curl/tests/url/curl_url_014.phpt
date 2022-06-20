--TEST--
CurlUrl::setPath() and CurlUrl::getPath()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl();

var_dump($url->getPath());

$url->setPath("path");
var_dump($url->getPath());

$url->setPath("Hello world", CurlUrl::URL_ENCODE);
var_dump($url->getPath());
var_dump($url->getPath(CurlUrl::URL_DECODE));

$url->setPath(NULL);
var_dump($url->getPath());

?>
--EXPECT--
string(1) "/"
string(4) "path"
string(13) "Hello%20world"
string(11) "Hello world"
string(1) "/"
