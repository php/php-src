--TEST--
CurlUrl::setPassword() and CurlUrl::getPassword()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl('https://www.php.net');

var_dump($url->getPassword());

$url->setPassword("foobar");
var_dump($url->getPassword());

$url->setPassword("foo bar", CurlUrl::URLENCODE);
var_dump($url->getPassword());
var_dump($url->getPassword(CurlUrl::URLDECODE));

$url->setPassword(NULL);
var_dump($url->getPassword());

?>
--EXPECT--
NULL
string(6) "foobar"
string(9) "foo%20bar"
string(7) "foo bar"
NULL
