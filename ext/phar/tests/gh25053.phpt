--TEST--
GH-25053 (Phar truncates entries of 4 GiB or more without error/warning)
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--INI--
phar.readonly=0
--FILE--
<?php
$src = __DIR__ . '/gh25053.big';
$f = fopen($src, 'w');
ftruncate($f, 2**32 + 1);
fclose($f);

$tar = new PharData(__DIR__ . '/gh25053-1.tar');
try {
    $tar->addFile($src, 'big');
} catch (BadMethodCallException $e) {
    echo $e->getMessage(), "\n";
}

$tar = new PharData(__DIR__ . '/gh25053-2.tar');
try {
    $tar->buildFromIterator(new ArrayIterator(['big' => $src]));
} catch (UnexpectedValueException $e) {
    echo str_replace($src, 'gh25053.big', $e->getMessage()), "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25053.big');
@unlink(__DIR__ . '/gh25053-1.tar');
@unlink(__DIR__ . '/gh25053-2.tar');
?>
--EXPECT--
Entry big is too large to be stored in a phar archive
Iterator ArrayIterator returned a file that is too large "gh25053.big"
