--TEST--
Bug #72858 shm_attach null dereference
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (4 < PHP_INT_SIZE) { print "skip 32-bit only"; }
if (substr(PHP_OS, 0, 3) != "WIN") { print "skip windows only"; }
?>
--FILE--
<?php

$v1=100;
$v2=0xffffffff / 4 + 0x1337;
shm_attach($v1,$v2);

?>
==DONE==
--EXPECTF--	
Warning: shm_attach(): failed for key 0x64: Not enough space in %s on line %d
==DONE==
