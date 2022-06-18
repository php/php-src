--TEST--
CurlUrl::setFragment() and CurlUrl::getFragment()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl();

var_dump($url->getFragment());

$url->setFragment("foobar");
var_dump($url->getFragment());

$url->setFragment("foo bar", CurlUrl::URLENCODE);
var_dump($url->getFragment());
var_dump($url->getFragment(CurlUrl::URLDECODE));

$url->setFragment(NULL);
var_dump($url->getFragment());

?>
--EXPECT--
NULL
string(6) "foobar"
string(9) "foo%20bar"
string(7) "foo bar"
NULL
