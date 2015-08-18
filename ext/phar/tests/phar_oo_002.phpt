--TEST--
Phar object: iterator & entries
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
if (version_compare(PHP_VERSION, "6.0", "==")) die("skip pre-unicode version of PHP required");
if (!extension_loaded("spl")) die("skip SPL not available");
?>
--INI--
phar.readonly=1
phar.require_hash=0
--FILE--
<?php

require_once 'files/phar_oo_test.inc';

$phar = new Phar($fname);
$phar->setInfoClass('SplFileInfo');
foreach($phar as $name => $ent)
{
	var_dump(str_replace(str_replace('\\', '/', dirname(__FILE__)), '*', $name));
	var_dump($ent->getFilename());
	var_dump($ent->getSize());
	var_dump($ent->getType());
	var_dump($ent->isWritable());
	var_dump($ent->isReadable());
	var_dump($ent->isExecutable());
	var_dump($ent->isFile());
	var_dump($ent->isDir());
	var_dump($ent->isLink());
	var_dump($ent->getCTime());
	var_dump($ent->getMTime());
	var_dump($ent->getATime());
}

echo "==RECURSIVE==\n";

$phar = new Phar($fname);
foreach(new RecursiveIteratorIterator($phar) as $name => $ent)
{
	var_dump(str_replace(str_replace('\\', '/', dirname(__FILE__)), '*', $name));
	var_dump(str_replace('\\', '/', $ent->getFilename()));
	var_dump($ent->getCompressedSize());
	var_dump($ent->isCRCChecked());
	var_dump($ent->isCRCChecked() ? $ent->getCRC32() : NULL);
	var_dump($ent->getPharFlags());
}

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/files/phar_oo_002.phar.php');
__halt_compiler();
?>
--EXPECTF--
string(41) "phar://*/files/phar_oo_002.phar.php%ca.php"
string(5) "a.php"
int(32)
string(4) "file"
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
int(%d)
int(%d)
int(%d)
string(37) "phar://*/files/phar_oo_002.phar.php%cb"
string(1) "b"
int(0)
string(3) "dir"
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
int(%d)
int(%d)
int(%d)
string(41) "phar://*/files/phar_oo_002.phar.php%cb.php"
string(5) "b.php"
int(32)
string(4) "file"
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
int(%d)
int(%d)
int(%d)
string(41) "phar://*/files/phar_oo_002.phar.php%ce.php"
string(5) "e.php"
int(32)
string(4) "file"
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
int(%d)
int(%d)
int(%d)
==RECURSIVE==
string(41) "phar://*/files/phar_oo_002.phar.php%ca.php"
string(5) "a.php"
int(32)
bool(false)
NULL
int(0)
string(43) "phar://*/files/phar_oo_002.phar.php/b%cc.php"
string(5) "c.php"
int(34)
bool(false)
NULL
int(0)
string(43) "phar://*/files/phar_oo_002.phar.php/b%cd.php"
string(5) "d.php"
int(34)
bool(false)
NULL
int(0)
string(41) "phar://*/files/phar_oo_002.phar.php%cb.php"
string(5) "b.php"
int(32)
bool(false)
NULL
int(0)
string(41) "phar://*/files/phar_oo_002.phar.php%ce.php"
string(5) "e.php"
int(32)
bool(false)
NULL
int(0)
===DONE===
