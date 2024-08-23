--TEST--
GH-15547 - curl_multi_select overflow on timeout argument
--EXTENSIONS--
curl
--FILE--
<?php

$mh = curl_multi_init();
var_dump(curl_multi_select($mh, -2500000));
var_dump(curl_multi_strerror(curl_multi_errno($mh)));
curl_multi_close($mh);
$mh = curl_multi_init();
var_dump(curl_multi_select($mh, 2500000));
var_dump(curl_multi_strerror(curl_multi_errno($mh)));
curl_multi_close($mh);
$mh = curl_multi_init();
var_dump(curl_multi_select($mh, 1000000));
var_dump(curl_multi_strerror(curl_multi_errno($mh)));
?>
--EXPECTF--
Warning: curl_multi_select(): timeout must be between 0 and %d in %s on line %d
int(-1)
%s

Warning: curl_multi_select(): timeout must be between 0 and %d in %s on line %d
int(-1)
%s
int(0)
string(8) "No error"
