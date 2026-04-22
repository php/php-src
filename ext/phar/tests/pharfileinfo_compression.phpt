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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$b->compress(25);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
$tar = $phar->convertToData(Phar::TAR);

$c = $tar['a/b'];
try {
	$c->compress(Phar::GZ);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
try {
	$phar['a']->compress(Phar::GZ);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
ini_set('phar.readonly', 1);
try {
	$b->compress(Phar::GZ);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
ini_set('phar.readonly', 0);
try {
	$phar['a']->decompress();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage() . "\n";
}
var_dump($b->decompress());
var_dump($b->decompress());

?>
--CLEAN--
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.phar'); ?>
<?php unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar'); ?>
--EXPECT--
ValueError: PharFileInfo::isCompressed(): Argument #1 ($compression) must be one of Phar::GZ or Phar::BZ2
ValueError: PharFileInfo::compress(): Argument #1 ($compression) must be one of Phar::GZ or Phar::BZ2
BadMethodCallException: Cannot compress with Gzip compression, not possible with tar-based phar archives
BadMethodCallException: Phar entry is a directory, cannot set compression
BadMethodCallException: Phar is readonly, cannot change compression
bool(true)
bool(true)
bool(true)
bool(true)
decompress
BadMethodCallException: Phar is readonly, cannot decompress
BadMethodCallException: Phar entry is a directory, cannot set compression
bool(true)
bool(true)
