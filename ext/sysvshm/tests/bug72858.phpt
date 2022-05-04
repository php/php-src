--TEST--
Bug #72858 shm_attach null dereference
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
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
==DONE==
