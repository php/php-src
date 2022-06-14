--TEST--
curl_url_get() and CurlUrl::get()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
$url = curl_url('https://www.example.com/');
echo curl_url_get($url, CURLUPART_URL), PHP_EOL;
echo curl_url_get($url, CURLUPART_PORT, CURLU_DEFAULT_PORT), PHP_EOL;

echo $url->get(CURLUPART_URL), PHP_EOL;
echo $url->get(CURLUPART_PORT, CURLU_DEFAULT_PORT), PHP_EOL;
?>
--EXPECT--
https://www.example.com/
443
https://www.example.com/
443
