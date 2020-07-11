--TEST--
curl_multi_setopt basic test
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
	    exit("skip curl extension not loaded");
}
?>
--FILE--
<?php

$mh = curl_multi_init();
var_dump(curl_multi_setopt($mh, CURLMOPT_PIPELINING, 0));
var_dump(curl_multi_setopt($mh, -1, 0));

?>
--EXPECTF--
bool(true)

Warning: curl_multi_setopt(): Invalid curl multi configuration option in %s on line %d
bool(false)
