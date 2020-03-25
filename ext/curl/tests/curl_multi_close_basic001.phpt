--TEST--
curl_multi_close return false when supplied resource not valid cURL multi handle
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
try {
    $bad_mh_close_result = curl_multi_close($cmh);
    var_dump($bad_mh_close_result);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
resource(%d) of type (curl_multi)
NULL
resource(%d) of type (Unknown)
curl_multi_close(): supplied resource is not a valid cURL Multi Handle resource
