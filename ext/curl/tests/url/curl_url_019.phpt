--TEST--
CurlUrl::setOptions() and CurlUrl::getOptions()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl('https://www.php.net');

var_dump($url->getOptions());

$url->setOptions("foobar");
var_dump($url->getOptions());

$url->setOptions("foo bar", CurlUrl::URLENCODE);
var_dump($url->getOptions());
var_dump($url->getOptions(CurlUrl::URLDECODE));

$url->setOptions(NULL);
var_dump($url->getOptions());

?>
--EXPECT--
NULL
string(6) "foobar"
string(9) "foo%20bar"
string(7) "foo bar"
NULL
