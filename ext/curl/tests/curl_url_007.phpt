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
echo new CurlUrl('https://www.php.net/');
?>
--EXPECT--
https://www.php.net/
