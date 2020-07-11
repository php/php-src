--TEST--
Phar: PharFileInfo::setMetadata/delMetadata extra code coverage
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$pname = 'phar://' . $fname;

$phar = new Phar($fname);

$phar['a/b'] = 'hi there';
$tar = $phar->convertToData(Phar::TAR);

$b = $phar['a/b'];
try {
$phar['a']->setMetadata('hi');
} catch (Exception $e) {
echo $e->getMessage(), "\n";
}
try {
$phar['a']->delMetadata();
} catch (Exception $e) {
echo $e->getMessage(), "\n";
}
ini_set('phar.readonly', 1);
try {
$b->setMetadata('hi');
} catch (Exception $e) {
echo $e->getMessage(), "\n";
}
try {
$b->delMetadata();
} catch (Exception $e) {
echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar'); ?>
--EXPECT--
Phar entry is a temporary directory (not an actual entry in the archive), cannot set metadata
Phar entry is a temporary directory (not an actual entry in the archive), cannot delete metadata
Write operations disabled by the php.ini setting phar.readonly
Write operations disabled by the php.ini setting phar.readonly
