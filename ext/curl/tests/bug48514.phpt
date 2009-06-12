--TEST--
Bug #48514 (cURL extension uses same resource name for simple and multi APIs)
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
resource(4) of type (curl)
%string|unicode%(4) "curl"
resource(5) of type (curl_multi)
%string|unicode%(10) "curl_multi"
