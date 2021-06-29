--TEST--
curl_strerror test constants not defined with < 7.16.1
--SKIPIF--
<?php
if (!extension_loaded('curl')) {
    exit('skip curl extension not loaded');
}
$curl_version = curl_version();
if ($curl_version['version_number'] >= 0x071001) {
	exit('skip: test works only with curl < 7.16.1');
}
?>
--FILE--
<?php

var_dump(strtolower(curl_strerror(CURLE_REMOTE_FILE_NOT_FOUND)));

?>
--EXPECTF--
Warning: Use of undefined constant CURLE_SEND_FAIL_REWIND - assumed 'CURLE_SEND_FAIL_REWIND' (this will throw an Error in a future version of PHP) in %s on line %d
Warning: curl_strerror() expects parameter 1 to be integer, string given in %s on line %d
