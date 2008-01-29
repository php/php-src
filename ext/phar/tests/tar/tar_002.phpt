--TEST--
Phar: tar-based phar corrupted 2
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
include dirname(__FILE__) . '/make_invalid_tar.php.inc';
$a = new corrupter(dirname(__FILE__) . '/tar_002.phar', 'none');
$a->init();
$a->addFile('tar_002.phpt', __FILE__);
$a->close();

$a = fopen('phar://' . dirname(__FILE__) . '/tar_002.phar/tar_002.phpt', 'rb');
try {
$a = new Phar(dirname(__FILE__) . '/tar_002.phar');
echo "should not execute\n";
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php
@unlink(dirname(__FILE__) . '/tar_002.phar');
?>
--EXPECTF--
Warning: fopen(phar://%star_002.phar/tar_002.phpt): failed to open stream: phar error: "%star_002.phar" is a corrupted tar file in %star_002.php on line %d
Cannot open phar file '%star_002.phar' with alias '(null)': phar error: "%star_002.phar" is a corrupted tar file
===DONE===