--TEST--
CurlUrlException getMessage
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (curl_version()['version_number'] < 0x075000) die('skip requires curl >= 7.80.0');
?>
--FILE--
<?php

try {
	new CurlUrl('foobar://php.net');
} catch (CurlUrlException $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
This libcurl build doesn't support the given URL scheme
