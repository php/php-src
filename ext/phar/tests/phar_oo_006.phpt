--TEST--
Phar object: array access
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.require_hash=0
--FILE--
<?php

require_once 'files/phar_oo_test.inc';

class MyFile extends SplFileObject
{
	function __construct($what)
	{
		echo __METHOD__ . "($what)\n";
		parent::__construct($what);
	}
}

$phar = new Phar($fname);
try
{
	$phar->setFileClass('SplFileInfo');
}
catch (UnexpectedValueException $e)
{
	echo $e->getMessage() . "\n";
}
$phar->setInfoClass('MyFile');

echo $phar['a.php']->getFilename() . "\n";
echo $phar['b/c.php']->getFilename() . "\n";
echo $phar['b.php']->getFilename() . "\n";

?>
===DONE===
--CLEAN--
<?php
unlink(dirname(__FILE__) . '/files/phar_oo_006.phar.php');
__halt_compiler();
?>
--EXPECTF--
SplFileInfo::setFileClass() expects parameter 1 to be a class name derived from SplFileObject, 'SplFileInfo' given
MyFile::__construct(phar://%s/a.php)
a.php
MyFile::__construct(phar://%s/b/c.php)
c.php
MyFile::__construct(phar://%s/b.php)
b.php
===DONE===
