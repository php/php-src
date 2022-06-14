--TEST--
CurlUrl constructor
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php

$url1 = new CurlUrl();
$url2 = new CurlUrl(null);
$url3 = new CurlUrl('https://www.google.com');

var_dump($url1, $url2, $url3);
?>
--EXPECT--
object(CurlUrl)#1 (0) {
}
object(CurlUrl)#2 (0) {
}
object(CurlUrl)#3 (0) {
}
