--TEST--
clone CurlUrl object
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
$url = new CurlUrl('https://www.example.com/');
$url2 = clone $url;

$url->setHost('www.php.net');
var_dump($url->get());

unset($url);

var_dump($url2->get());
?>
--EXPECT--
string(20) "https://www.php.net/"
string(24) "https://www.example.com/"
