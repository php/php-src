--TEST--
Bug #72202 (curl_close doesn't close cURL handle)
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
    exit("skip curl extension not loaded");
}
?>
--FILE--
<?php
$a = fopen(__FILE__, "r");
$b = $a;
var_dump($a, $b);
fclose($a);
var_dump($a, $b);
unset($a, $b);

$a = curl_init();
$b = $a;
var_dump($a, $b);
curl_close($a);
var_dump($a, $b);
unset($a, $b);
?>
--EXPECTF--
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (Unknown)
resource(%d) of type (Unknown)
object(CurlHandle)#1 (0) {
}
object(CurlHandle)#1 (0) {
}
object(CurlHandle)#1 (0) {
}
object(CurlHandle)#1 (0) {
}
