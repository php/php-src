--TEST--
CurlUrl::setUser() and CurlUrl::getUser()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl('https://www.php.net');

var_dump($url->getUser());

$url->setUser("foobar");
var_dump($url->getUser());

$url->setUser("foo bar", CurlUrl::URLENCODE);
var_dump($url->getUser());
var_dump($url->getUser(CurlUrl::URLDECODE));

$url->setUser(NULL);
var_dump($url->getUser());

?>
--EXPECT--
NULL
string(6) "foobar"
string(9) "foo%20bar"
string(7) "foo bar"
NULL
