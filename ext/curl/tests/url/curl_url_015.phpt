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

$url->setQuery('foo=bar');
var_dump($url->getQuery());

$url->setQuery('foo=bar baz', CurlUrl::URLENCODE);
var_dump($url->getQuery());
var_dump($url->getQuery(CurlUrl::URLDECODE));

$url->setQuery(NULL);
var_dump($url->getQuery());

$url->setQuery('foo=bar');
$url->setQuery('bar=baz&baz=qux', CurlUrl::APPENDQUERY);
var_dump($url->getQuery());

$url->setQuery(NULL);
$url->setQuery('foo=bar');
$url->setQuery('bar=baz&baz=qux', CurlUrl::APPENDQUERY | CurlUrl::URLENCODE);
var_dump($url->getQuery());

?>
--EXPECT--
NULL
string(7) "foo=bar"
string(13) "foo%3dbar+baz"
string(11) "foo=bar baz"
NULL
string(23) "foo=bar&bar=baz&baz=qux"
string(27) "foo=bar&bar=baz%26baz%3dqux"
