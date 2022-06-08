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
$url = curl_url('https://www.example.com/');
$url2 = clone $url;
echo curl_url_get($url, CURLUPART_URL);
?>
--EXPECT--
https://www.example.com/
