--TEST--
Phar: delete a file within a zip-based .phar (confirm disk file is changed)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

$fname = dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip';
$alias = 'phar://' . $fname;

$phar = new Phar($fname);
$phar['a.php'] = '<?php echo "This is a\n"; ?>';
$phar['b.php'] = '<?php echo "This is b\n"; ?>';
$phar['b/c.php'] = '<?php echo "This is b/c\n"; ?>';
$phar->setStub('<?php __HALT_COMPILER(); ?>');
$phar->stopBuffering();

if (function_exists("opcache_get_status")) {
	$status = opcache_get_status();
	if ($status["opcache_enabled"]) {
		ini_set("opcache.revalidate_freq", "0");
		sleep(2);
	}
}

include $alias . '/a.php';
include $alias . '/b.php';
include $alias . '/b/c.php';

$md5 = md5_file($fname);
unlink($alias . '/b/c.php');
clearstatcache();
$md52 = md5_file($fname);
if ($md5 == $md52) echo 'file was not modified';
?>
===AFTER===
<?php
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip/a.php';
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip/b.php';
include 'phar://' . dirname(__FILE__) . '/' . basename(__FILE__, '.php') . '.phar.zip/b/c.php';
?>

===DONE===
--CLEAN--
<?php unlink(dirname(__FILE__) . '/' . basename(__FILE__, '.clean.php') . '.phar.zip'); ?>
--EXPECTF--
This is a
This is b
This is b/c
===AFTER===
This is a
This is b

Warning: include(%sdelete_in_phar_confirm.phar.zip/b/c.php): failed to open stream: phar error: "b/c.php" is not a file in phar "%sdelete_in_phar_confirm.phar.zip" in %sdelete_in_phar_confirm.php on line %d

Warning: include(): Failed opening 'phar://%sdelete_in_phar_confirm.phar.zip/b/c.php' for inclusion (include_path='%s') in %sdelete_in_phar_confirm.php on line %d

===DONE===
