--TEST--
Bug #71144 (Sementation fault when using cURL with ZTS)
--DESCRIPTION--
Since Curl 7.62, CURLOPT_DNS_USE_GLOBAL_CACHE has no effect, and is
silently ignored.
--EXTENSIONS--
curl
--FILE--
<?php
$ch = curl_init();
var_dump(curl_setopt($ch, CURLOPT_DNS_USE_GLOBAL_CACHE, 1));
?>
--EXPECTF--
bool(true)
