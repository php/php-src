--TEST--
Bug #72858 shm_attach null dereference
--EXTENSIONS--
sysvshm
--SKIPIF--
<?php

if (4 < PHP_INT_SIZE) { print "skip 32-bit only"; }
if (substr(PHP_OS, 0, 3) != "WIN") { print "skip windows only"; }
if (getenv('GITHUB_ACTIONS')) die('skip on GitHub actions');
?>
--FILE--
<?php

$v1=100;
$v2=0x3fffffff + 0x1337;
shm_attach($v1,$v2);

?>
--EXPECTF--
Warning: shm_attach(): Failed for key 0x64: Not enough space in %s on line %d
