--TEST--
Phar: PharFileInfo::setMetadata/delMetadata extra code coverage
--EXTENSIONS--
phar
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
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
$phar['a']->delMetadata();
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
ini_set('phar.readonly', 1);
try {
$b->setMetadata('hi');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
$b->delMetadata();
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar'); ?>
--EXPECT--
BadMethodCallException: Phar entry is a temporary directory (not an actual entry in the archive), cannot set metadata
BadMethodCallException: Phar entry is a temporary directory (not an actual entry in the archive), cannot delete metadata
BadMethodCallException: Write operations disabled by the php.ini setting phar.readonly
BadMethodCallException: Write operations disabled by the php.ini setting phar.readonly
