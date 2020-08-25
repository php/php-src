--TEST--
curl_share_setopt basic test
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
	    exit("skip curl extension not loaded");
}
?>
--FILE--
<?php

$sh = curl_share_init();
var_dump(curl_share_setopt($sh, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE));
var_dump(curl_share_setopt($sh, CURLSHOPT_UNSHARE, CURL_LOCK_DATA_DNS));
var_dump(curl_share_setopt($sh, -1, 0));

?>
--EXPECTF--
bool(true)
bool(true)

Warning: curl_share_setopt(): Invalid curl share configuration option in %s on line %d
bool(false)
