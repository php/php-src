--TEST--
curl_multi_close return false when suplied resorce not valid cURL multi handle
--SKIPIF--
<?php
if (!extension_loaded('curl')) print 'skip';
?>
--FILE--
<?php
$cmh = curl_multi_init();
var_dump($cmh);
$multi_close_result = curl_multi_close($cmh);
var_dump($multi_close_result);
var_dump($cmh);
$bad_mh_close_result = curl_multi_close($cmh);
var_dump($bad_mh_close_result);
?>
===DONE===
--EXPECTF--
resource(%d) of type (curl_multi)
NULL
resource(%d) of type (Unknown)

Warning: curl_multi_close(): supplied resource is not a valid cURL Multi Handle resource in %s on line %d
bool(false)
===DONE===
