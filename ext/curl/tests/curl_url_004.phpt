--TEST--
curl_url_set() function
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
$url = curl_url();
var_dump(curl_url_set($url, CURLUPART_HOST, 'www.example.com'));
var_dump(curl_url_set($url, CURLUPART_SCHEME, 'ftp'));
echo curl_url_get($url, CURLUPART_URL);
?>
--EXPECT--
bool(true)
bool(true)
ftp://www.example.com/
