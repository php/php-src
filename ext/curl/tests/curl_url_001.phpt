--TEST--
curl_url() function
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x073e00) die('skip requires curl >= 7.62.0');
?>
--FILE--
<?php
var_dump(curl_url());
?>
--EXPECT--
object(CurlUrl)#1 (0) {
}
