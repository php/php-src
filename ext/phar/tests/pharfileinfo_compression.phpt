--TEST--
Phar: PharFileInfo compression-related methods
--EXTENSIONS--
phar
zlib
bz2
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.phar';
$pname = 'phar://' . $fname;

$phar = new Phar($fname);

$phar['a/b'] = 'hi there';

$b = $phar['a/b'];

try {
$b->isCompressed(25);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$b->compress(25);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
$tar = $phar->convertToData(Phar::TAR);

$c = $tar['a/b'];
try {
$c->compress(Phar::GZ);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
try {
$phar['a']->compress(Phar::GZ);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
ini_set('phar.readonly', 1);
try {
$b->compress(Phar::GZ);
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
ini_set('phar.readonly', 0);
var_dump($b->compress(Phar::GZ));
var_dump($b->compress(Phar::GZ));
var_dump($b->compress(Phar::BZ2));
var_dump($b->compress(Phar::BZ2));

echo "decompress\n";

ini_set('phar.readonly', 1);
try {
$phar['a/b']->decompress();
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
ini_set('phar.readonly', 0);
try {
$phar['a']->decompress();
} catch (Exception $e) {
echo $e->getMessage() . "\n";
}
var_dump($b->decompress());
var_dump($b->decompress());

?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar'); ?>
--EXPECT--
Unknown compression type specified
Unknown compression type specified
Cannot compress with Gzip compression, not possible with tar-based phar archives
Phar entry is a directory, cannot set compression
Phar is readonly, cannot change compression
bool(true)
bool(true)
bool(true)
bool(true)
decompress
Phar is readonly, cannot decompress
Phar entry is a directory, cannot set compression
bool(true)
bool(true)
