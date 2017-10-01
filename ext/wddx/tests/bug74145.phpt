--TEST--
Bug #74145 (wddx parsing empty boolean tag leads to SIGSEGV)
--SKIPIF--
<?php
if (!extension_loaded("wddx")) print "skip";
?>
--FILE--
<?php
$data = file_get_contents(__DIR__ . '/bug74145.xml');
$wddx = wddx_deserialize($data);
var_dump($wddx);
?>
DONE
--EXPECTF--
NULL
DONE