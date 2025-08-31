--TEST--
curl_multi_close
--CREDITS--
Stefan Koopmanschap <stefan@php.net>
#testfest Utrecht 2009
--EXTENSIONS--
curl
--FILE--
<?php
$ch = curl_multi_init();
curl_multi_close($ch);
var_dump($ch);
?>
--EXPECT--
object(CurlMultiHandle)#1 (0) {
}
