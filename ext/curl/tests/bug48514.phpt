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
var_dump(get_resource_type($ch1));

$ch2 = curl_multi_init();
var_dump($ch2);
var_dump(get_resource_type($ch2));

?>
--EXPECTF--
resource(%d) of type (curl)
string(4) "curl"
resource(%d) of type (curl_multi)
string(10) "curl_multi"
