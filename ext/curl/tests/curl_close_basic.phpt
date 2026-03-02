--TEST--
curl_close
--CREDITS--
Stefan Koopmanschap <stefan@php.net>
#testfest Utrecht 2009
--EXTENSIONS--
curl
--FILE--
<?php
$ch = curl_init();
curl_close($ch);
var_dump($ch);
?>
--EXPECTF--
Deprecated: Function curl_close() is deprecated since 8.5, as it has no effect since PHP 8.0 in %s on line %d
object(CurlHandle)#%d (0) {
}
