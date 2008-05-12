--TEST--
Phar: tar-based phar corrupted 2
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
include dirname(__FILE__) . '/files/make_invalid_tar.php.inc';

$tar = new corrupter(dirname(__FILE__) . '/tar_002.phar.tar', 'none');
$tar->init();
$tar->addFile('tar_002.phpt', __FILE__);
$tar->close();

$tar = fopen('phar://' . dirname(__FILE__) . '/tar_002.phar.tar/tar_002.phpt', 'rb');

try {
	$phar = new Phar(dirname(__FILE__) . '/tar_002.phar.tar');
	echo "should not execute\n";
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_002.phar.tar');
?>
--EXPECTF--
Warning: fopen(phar://%star_002.phar.tar/tar_002.phpt): failed to open stream: phar error: "%star_002.phar.tar" is a corrupted tar file (truncated) in %star_002.php on line 9
phar error: "%star_002.phar.tar" is a corrupted tar file (truncated)
===DONE===
