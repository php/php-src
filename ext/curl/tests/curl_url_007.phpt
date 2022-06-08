--TEST--
CurlUrl to string conversion
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
echo curl_url('https://www.example.com/');
?>
--EXPECT--
https://www.example.com/
