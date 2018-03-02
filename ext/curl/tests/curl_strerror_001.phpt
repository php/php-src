--TEST--
curl_strerror basic test
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
	    exit("skip curl extension not loaded");
}
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x070c00) {
	exit("skip: test works only with curl >= 7.12.0");
}
?>
--FILE--
<?php

var_dump(strtolower(curl_strerror(CURLE_OK)));
var_dump(strtolower(curl_strerror(CURLE_UNSUPPORTED_PROTOCOL)));
var_dump(strtolower(curl_strerror(-1)));

?>
--EXPECT--
string(8) "no error"
string(20) "unsupported protocol"
string(13) "unknown error"
