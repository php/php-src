--TEST--
curl_multi_close
--CREDITS--
Stefan Koopmanschap <stefan@php.net>
#testfest Utrecht 2009
--SKIPIF--
<?php
if (!extension_loaded('curl')) print 'skip';
?>
--FILE--
<?php
$ch = curl_multi_init();
curl_multi_close($ch);
var_dump($ch);
?>
===DONE===
--EXPECTF--
resource(%d) of type (Unknown)
===DONE===
