--TEST--
Phar::chmod zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("zip")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include dirname(__FILE__) . '/tarmaker.php.inc';
$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.1.phar.php';
$pname = 'phar://hio';

$a = new tarmaker($fname, 'none');
$a->init();
$a->addFile('a.php', '<?php echo "This is a\n"; include "'.$pname.'/b.php"; ?>');
$a->addFile('.phar/alias.txt', 'hio');
$a->mkDir('test');
$a->close();

try {
	$a = new Phar($fname);
	var_dump($a['a.php']->isExecutable());
	$a['a.php']->chmod(0777);
	var_dump($a['a.php']->isExecutable());
	$a['a.php']->chmod(0666);
	var_dump($a['a.php']->isExecutable());
	echo "test dir\n";
	var_dump($a['test']->isDir());
	var_dump($a['test']->isReadable());
	$a['test']->chmod(0000);
	var_dump($a['test']->isReadable());
	$a['test']->chmod(0666);
	var_dump($a['test']->isReadable());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.1.phar.php');
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
test dir
bool(true)
bool(true)
bool(false)
bool(true)
===DONE===
