--TEST--
curl_version(): error conditions
--SKIPIF--
<?php
if (!extension_loaded('curl')) die('skip curl extension not available');
?>
--FILE--
<?php
curl_version(CURLVERSION_NOW);
curl_version(0);
?>
--EXPECTF--
Deprecated: curl_version(): The $version parameter is deprecated in %s on line %d

Warning: curl_version(): $version argument ignored in %s on line %d
