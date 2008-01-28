--TEST--
Phar::chmod zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
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
ini_set('phar.readonly', 1);

try {
	$a = new Phar($fname);
	var_dump($a['a.php']->isExecutable());
	$a['a.php']->chmod(0777);
	var_dump($a['a.php']->isExecutable());
	$a['a.php']->chmod(0666);
	var_dump($a['a.php']->isExecutable());
	echo "test dir\n";
	var_dump($a['test']->isExecutable());
	$a['test']->chmod(0777);
	var_dump($a['test']->isExecutable());
	$a['test']->chmod(0666);
	var_dump($a['test']->isExecutable());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.1.phar.php');
?>
--EXPECTF--
bool(false)
Cannot modify permissions for file "a.php" in phar "%sa.1.phar.php", write operations are prohibited
===DONE===
