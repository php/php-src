--TEST--
curl_close
--CREDITS--
Stefan Koopmanschap <stefan@php.net>
#testfest Utrecht 2009
--SKIPIF--
<?php
if (!extension_loaded('curl')) print 'skip';
?>
--FILE--
<?php
$ch = curl_init();
curl_close($ch);
var_dump($ch);
?>
--EXPECTF--
resource(%d) of type (Unknown)
