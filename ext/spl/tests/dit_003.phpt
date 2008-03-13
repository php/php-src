--TEST--
SPL: FilesystemIterator and foreach
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
$count = 0;
foreach(new FilesystemIterator('CVS') as $ent)
{
	++$count;
}
var_dump($count > 0);
?>
===DONE===
--EXPECTF--
bool(true)
===DONE===
