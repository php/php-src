--TEST--
Phar: tar-based phar corrupted
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--FILE--
<?php
include dirname(__FILE__) . '/make_invalid_tar.php.inc';
$a = new corrupter(dirname(__FILE__) . '/tar_001.phar', 'none');
$a->init();
$a->addFile('tar_001.phpt', __FILE__);
$a->close();

$a = fopen('phar://' . dirname(__FILE__) . '/tar_001.phar/tar_001.phpt', 'rb');
try {
$a = new Phar(dirname(__FILE__) . '/tar_001.phar');
echo "should not execute\n";
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_001.phar');
?>
--EXPECTF--
Warning: fopen(phar://%star_001.phar/tar_001.phpt): failed to open stream: phar error: "%star_001.phar" is a corrupted tar file in %star_001.php on line %d
Cannot open phar file '%star_001.phar' with alias '(null)': phar error: "%star_001.phar" is a corrupted tar file
===DONE===