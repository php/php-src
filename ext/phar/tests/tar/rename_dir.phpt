--TEST--
Phar: rename_dir test tar-based
--EXTENSIONS--
phar
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

$tar->close();

include $fname;

echo file_get_contents($alias . '/a/x') . "\n";
rename($alias . '/a', $alias . '/b');
echo file_get_contents($alias . '/b/x') . "\n";
echo file_get_contents($alias . '/a/x') . "\n";
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar.tar'); ?>
--EXPECTF--
a
a

Warning: file_get_contents(phar://%srename_dir.phar.tar/a/x): Failed to open stream: phar error: "a/x" is not a file in phar "%srename_dir.phar.tar" in %srename_dir.php on line %d
