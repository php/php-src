--TEST--
CurlUrl::setZoneId() and CurlUrl::getZoneId()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x074100) die('skip requires curl >= 7.65.0');
?>
--FILE--
<?php

$url = new CurlUrl('https://www.php.net');

var_dump($url->getZoneId());

$url->setZoneId("foobar");
var_dump($url->getZoneId());

$url->setZoneId("foo bar", CurlUrl::URLENCODE);
var_dump($url->getZoneId());
var_dump($url->getZoneId(CurlUrl::URLDECODE));

$url->setZoneId(NULL);
var_dump($url->getZoneId());

?>
--EXPECT--
NULL
string(6) "foobar"
string(9) "foo%20bar"
string(7) "foo bar"
NULL
