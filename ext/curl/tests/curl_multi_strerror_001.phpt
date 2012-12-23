--TEST--
curl_multi_strerror basic test
--SKIPIF--
if (!extension_loaded("curl")) {
	    exit("skip curl extension not loaded");
}
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x070c00) {
	exit("skip: test works only with curl >= 7.12.0");
}
--FILE--
<?php

var_dump(curl_multi_strerror(CURLM_OK));
var_dump(curl_multi_strerror(CURLM_BAD_HANDLE));

?>
--EXPECTF--
string(8) "No error"
string(20) "Invalid multi handle"
