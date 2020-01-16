--TEST--
curl_share_errno and curl_share_strerror basic test
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
	    exit("skip curl extension not loaded");
}
?>
--FILE--
<?php

$sh = curl_share_init();
$errno = curl_share_errno($sh);
echo $errno . PHP_EOL;
echo curl_share_strerror($errno) . PHP_EOL;

@curl_share_setopt($sh, -1, -1);
$errno = curl_share_errno($sh);
echo $errno . PHP_EOL;
echo curl_share_strerror($errno) . PHP_EOL;
?>
--EXPECT--
0
No error
1
Unknown share option
