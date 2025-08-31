--TEST--
Bug #48514 (cURL extension uses same resource name for simple and multi APIs)
--EXTENSIONS--
curl
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
object(CurlHandle)#1 (0) {
}
string(10) "CurlHandle"
object(CurlMultiHandle)#2 (0) {
}
string(15) "CurlMultiHandle"
