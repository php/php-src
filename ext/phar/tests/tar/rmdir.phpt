--TEST--
Phar: rmdir test tar-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include __DIR__ . '/files/tarmaker.php.inc';
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar.tar';
$alias = 'phar://' . $fname;

$tar = new tarmaker($fname, 'none');
$tar->init();
$tar->addFile('.phar/stub.php', "<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>");

$files = array();
$files['a/x'] = 'a';

foreach ($files as $n => $file) {
    $tar->addFile($n, $file);
}
$tar->mkdir('a');

$tar->close();

include $fname;

echo file_get_contents($alias . '/a/x') . "\n";
var_dump(rmdir($alias . '/a'));
echo file_get_contents($alias . '/a/x') . "\n";
unlink($alias . '/a/x');
var_dump(rmdir($alias . '/a'));
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar'); ?>
--EXPECTF--
a

Warning: rmdir(): phar error: Directory not empty in %srmdir.php on line %d
bool(false)
a
bool(true)
