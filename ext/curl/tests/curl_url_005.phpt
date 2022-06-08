--TEST--
curl_url_set() function with error
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
$url = curl_url();
var_dump(curl_url_errno($url));
var_dump(curl_url_set($url, CURLUPART_SCHEME, 'foobar'));
var_dump(curl_url_errno($url) == CURLUE_UNSUPPORTED_SCHEME);
?>
--EXPECT--
int(0)
bool(false)
bool(true)
