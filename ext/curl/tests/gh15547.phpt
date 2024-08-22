--TEST--
GH-15547 - overflow on timeout argument
--EXTENSIONS--
curl
--FILE--
<?php

$mh = curl_multi_init();
var_dump(curl_multi_select($mh, -2500000));
var_dump(curl_multi_select($mh, 2500000));
var_dump(curl_multi_select($mh, 1000000));
?>
--EXPECTF--
Warning: curl_multi_select(): timeout must be between %s and %s in %s on line %d
int(-1)

Warning: curl_multi_select(): timeout must be between %s and %s in %s on line %d
int(-1)
int(0)
