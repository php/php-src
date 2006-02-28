--TEST--
Phar object: iterator & entries
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--FILE--
<?php

require_once 'phar_oo_test.inc';

$phar = new Phar($fname);
$phar->setInfoClass('SplFileInfo');
foreach($phar as $name => $ent)
{
	var_dump(str_replace(dirname(__FILE__), '*', $name));
	var_dump($ent->getFilename());
	var_dump($ent->getSize());
	var_dump($ent->getType());
	var_dump($ent->isWritable());
	var_dump($ent->isReadable());
	var_dump($ent->isExecutable());
	var_dump($ent->isFile());
	var_dump($ent->isDir());
	var_dump($ent->isLink());
}

echo "==RECURSIVE==\n";

$phar = new Phar($fname);
foreach(new RecursiveIteratorIterator($phar) as $name => $ent)
{
	var_dump(str_replace(dirname(__FILE__), '*', $name));
	var_dump($ent->getFilename());
	var_dump($ent->getCompressedSize());
	var_dump($ent->isCRCChecked());
	var_dump($ent->isCRCChecked() ? $ent->getCRC32() : NULL);
	var_dump($ent->getPharFlags());
}

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/phar_oo_test.phar.php');
__halt_compiler();
?>
--EXPECT--
string(36) "phar://*/phar_oo_test.phar.php/a.php"
string(5) "a.php"
int(32)
string(4) "file"
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
string(32) "phar://*/phar_oo_test.phar.php/b"
string(1) "b"
int(0)
string(3) "dir"
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
string(36) "phar://*/phar_oo_test.phar.php/b.php"
string(5) "b.php"
int(32)
string(4) "file"
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
string(36) "phar://*/phar_oo_test.phar.php/e.php"
string(5) "e.php"
int(32)
string(4) "file"
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
==RECURSIVE==
string(36) "phar://*/phar_oo_test.phar.php/a.php"
string(5) "a.php"
int(32)
bool(false)
NULL
int(0)
string(38) "phar://*/phar_oo_test.phar.php/b/c.php"
string(5) "c.php"
int(34)
bool(false)
NULL
int(0)
string(38) "phar://*/phar_oo_test.phar.php/b/d.php"
string(5) "d.php"
int(34)
bool(false)
NULL
int(0)
string(36) "phar://*/phar_oo_test.phar.php/b.php"
string(5) "b.php"
int(32)
bool(false)
NULL
int(0)
string(36) "phar://*/phar_oo_test.phar.php/e.php"
string(5) "e.php"
int(32)
bool(false)
NULL
int(0)
===DONE===
