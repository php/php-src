--TEST--
curl_multi_strerror basic test
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
        exit("skip curl extension not loaded");
}
?>
--FILE--
<?php

var_dump(strtolower(curl_multi_strerror(CURLM_OK)));
var_dump(strtolower(curl_multi_strerror(CURLM_BAD_HANDLE)));

?>
--EXPECT--
string(8) "no error"
string(20) "invalid multi handle"
