--TEST--
curl_multi_close return false when supplied resource not valid cURL multi handle
--EXTENSIONS--
curl
--FILE--
<?php
$cmh = curl_multi_init();
var_dump($cmh);
$multi_close_result = curl_multi_close($cmh);
var_dump($multi_close_result);
var_dump($cmh);
curl_multi_close($cmh);
?>
--EXPECT--
object(CurlMultiHandle)#1 (0) {
}
NULL
object(CurlMultiHandle)#1 (0) {
}
