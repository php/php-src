--TEST--
curl_strerror test constants from >= 7.16.1
--SKIPIF--
<?php
if (!extension_loaded('curl')) {
    exit('skip curl extension not loaded');
}
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x071001) {
	exit('skip: test works only with curl >= 7.16.1');
}
?>
--FILE--
<?php

var_dump(strtolower(curl_strerror(CURLE_REMOTE_FILE_NOT_FOUND)));
var_dump(strtolower(curl_strerror(CURLE_SSL_SHUTDOWN_FAILED)));

?>
--EXPECT--
string(21) "remote file not found"
string(38) "failed to shut down the ssl connection"
