--TEST--
CurlUrl::setScheme() and CurlUrl::getScheme()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl();

var_dump($url->getScheme());

$url->setScheme('https');
var_dump($url->getScheme());

try {
	$url->setScheme('foobar');
} catch (CurlUrlException $e) {
	var_dump($e->getCode() == CurlUrlException::UNSUPPORTED_SCHEME);
}
var_dump($url->getScheme());

try {
	$url->setScheme('foobar', CurlUrl::NON_SUPPORT_SCHEME);
} catch (CurlUrlException $e) {
	var_dump($e->getCode() == CurlUrlException::UNSUPPORTED_SCHEME);
}
var_dump($url->getScheme());
$url->setScheme(null);
var_dump($url->getScheme());

?>
--EXPECT--
NULL
string(5) "https"
bool(true)
string(5) "https"
string(6) "foobar"
NULL
