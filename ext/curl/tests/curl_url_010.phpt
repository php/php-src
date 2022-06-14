--TEST--
curl_url_strerror()
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x075000) die('skip requires curl >= 7.80.0');
?>
--FILE--
<?php

echo curl_url_strerror(CURLUE_BAD_SCHEME);

?>
--EXPECT--
Bad scheme
