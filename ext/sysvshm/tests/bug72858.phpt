--TEST--
Bug #72858 shm_attach null dereference
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (substr(PHP_OS, 0, 3) != "WIN") { print "skip windows only"; }
?>
--FILE--
<?php

$v1 = 100;
$v2 = PHP_INT_MAX;
shm_attach($v1, $v2);

?>
--EXPECTF--
Warning: shm_attach(): Failed for key 0x64: No error in %s on line %d
