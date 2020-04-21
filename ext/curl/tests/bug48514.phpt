--TEST--
Bug #48514 (cURL extension uses same resource name for simple and multi APIs)
--SKIPIF--
<?php

if (!extension_loaded('curl')) {
	exit("skip curl extension not loaded");
}

?>
--FILE--
<?php

$ch1 = curl_init();
var_dump($ch1);
var_dump($ch1::class);

$ch2 = curl_multi_init();
var_dump($ch2);
var_dump($ch2::class);

?>
--EXPECT--
object(Curl)#1 (0) {
}
string(4) "Curl"
object(CurlMulti)#2 (0) {
}
string(9) "CurlMulti"
