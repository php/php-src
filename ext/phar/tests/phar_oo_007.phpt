--TEST--
Phar object: access through SplFileObject
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
	function __construct($name)
	{
		echo __METHOD__ . "(" . str_replace(str_replace('\\', '/', dirname(__FILE__)), '*', $name) . ")\n";
		parent::__construct($name);
	}
}

$phar = new Phar($fname);
$phar->setInfoClass('MyFile');

$f = $phar['a.php'];

$s = $f->fstat();

var_dump($s['atime']);
var_dump($s['ctime']);
var_dump($s['mtime']);

var_dump($f->ftell());
var_dump($f->eof());
var_dump($f->fgets());
var_dump($f->eof());
var_dump($f->fseek(20));
var_dump($f->ftell());
var_dump($f->fgets());
var_dump($f->rewind());
var_dump($f->ftell());
var_dump($f->fgets());
var_dump($f->ftell());

?>
===AGAIN===
<?php

$f = $phar['a.php'];

var_dump($f->ftell());
var_dump($f->eof());
var_dump($f->fgets());
var_dump($f->eof());

//unset($f); without unset we check for working refcounting

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/files/phar_oo_007.phar.php');
__halt_compiler();
?>
--EXPECTF--
MyFile::__construct(phar://*/files/phar_oo_007.phar.php/a.php)
int(%d)
int(%d)
int(%d)
int(0)
bool(false)
string(32) "<?php echo "This is a.php\n"; ?>"
bool(true)
int(0)
int(20)
string(12) "a.php\n"; ?>"
NULL
int(0)
string(32) "<?php echo "This is a.php\n"; ?>"
int(32)
===AGAIN===
MyFile::__construct(phar://*/files/phar_oo_007.phar.php/a.php)
int(0)
bool(false)
string(32) "<?php echo "This is a.php\n"; ?>"
bool(true)
===DONE===
