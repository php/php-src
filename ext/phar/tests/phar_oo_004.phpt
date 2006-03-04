--TEST--
Phar object: entry & openFile()
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php

require_once 'phar_oo_test.inc';

$it = new DirectoryIterator('phar://'.$fname);

foreach($it as $name => $ent)
{
	var_dump($name);
	var_dump($ent->getFilename());
	var_dump($ent->isDir());
}

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/phar_oo_test.phar.php');
__halt_compiler();
?>
--EXPECT--
int(0)
string(5) "a.php"
bool(false)
int(1)
string(1) "b"
bool(true)
int(2)
string(5) "b.php"
bool(false)
int(3)
string(5) "e.php"
bool(false)
===DONE===
