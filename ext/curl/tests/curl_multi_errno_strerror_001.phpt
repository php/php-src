--TEST--
curl_multi_errno and curl_multi_strerror basic test
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
	    exit("skip curl extension not loaded");
}
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x070f04) {
	exit("skip: test works only with curl >= 7.15.4");
}
?>
--FILE--
<?php

$mh = curl_multi_init();
$errno = curl_multi_errno($mh);
echo $errno . PHP_EOL;
echo curl_multi_strerror($errno) . PHP_EOL;

@curl_multi_setopt($mh, -1, -1);
$errno = curl_multi_errno($mh);
echo $errno . PHP_EOL;
echo curl_multi_strerror($errno) . PHP_EOL;
?>
--EXPECTF--
0
No error
6
Unknown option
