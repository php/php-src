--TEST--
CurlUrl::setPort() and CurlUrl::getPort()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url = new CurlUrl('https://www.php.net');

var_dump($url->getPort());
var_dump($url->getPort(CurlUrl::DEFAULT_PORT));

$url->setPort(443);
var_dump($url->getPort());

try {
	$url->setPort(0);
} catch (CurlUrlException $e) {
	var_dump($e->getCode() == CurlUrlException::BAD_PORT_NUMBER);
}
var_dump($url->getPort());
var_dump($url->getPort(CurlUrl::NO_DEFAULT_PORT));

$url->setPort(null);
var_dump($url->getPort());

?>
--EXPECT--
NULL
int(443)
int(443)
bool(true)
int(443)
NULL
NULL
