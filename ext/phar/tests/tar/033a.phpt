--TEST--
Phar::chmod tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
$alias = 'phar://hio';

$phar = new Phar($fname);
$phar['a.php'] = '<?php echo "This is a\n"; include "'.$alias.'/b.php"; ?>';
$phar->setAlias('hio');
$phar->addEmptyDir('test');
$phar->stopBuffering();
ini_set('phar.readonly', 1);

try {
	var_dump($phar['a.php']->isExecutable());
	$phar['a.php']->chmod(0777);
	var_dump($phar['a.php']->isExecutable());
	$phar['a.php']->chmod(0666);
	var_dump($phar['a.php']->isExecutable());
	echo "test dir\n";
	var_dump($phar['test']->isReadable());
	$phar['test']->chmod(0000);
	var_dump($phar['test']->isReadable());
	$phar['test']->chmod(0666);
	var_dump($phar['test']->isReadable());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar');
?>
--EXPECTF--
bool(false)
Cannot modify permissions for file "a.php" in phar "%s033a.phar.tar", write operations are prohibited
===DONE===
