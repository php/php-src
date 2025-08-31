--TEST--
Casting CurlHandle to int returns object ID
--EXTENSIONS--
curl
--FILE--
<?php

$handle1 = curl_init();
var_dump((int) $handle1);
$handle2 = curl_init();
var_dump((int) $handle2);

// NB: Unlike resource IDs, object IDs are reused.
unset($handle2);
$handle3 = curl_init();
var_dump((int) $handle3);

// Also works for CurlMultiHandle.
$handle4 = curl_multi_init();
var_dump((int) $handle4);

?>
--EXPECT--
int(1)
int(2)
int(2)
int(3)
