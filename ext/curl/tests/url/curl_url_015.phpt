--TEST--
CurlUrl::setQuery() and CurlUrl::getQuery()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl();

var_dump($url->getQuery());

$url->setQuery("foo=bar");
var_dump($url->getQuery());

$url->setQuery("foo=bar baz", CurlUrl::URLENCODE);
var_dump($url->getQuery());
var_dump($url->getQuery(CurlUrl::URLDECODE));

$url->setQuery(NULL);
var_dump($url->getQuery());

?>
--EXPECT--
NULL
string(7) "foo=bar"
string(13) "foo%3dbar+baz"
string(11) "foo=bar baz"
NULL
