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
--ENV--
TEST_TIMEOUT=600
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
var_dump(isset($tar['big']));

$tar = new PharData(__DIR__ . '/gh25053-2.tar');
try {
    $tar->buildFromIterator(new ArrayIterator(['big' => $src]));
} catch (BadMethodCallException $e) {
    echo $e->getMessage(), "\n";
}
var_dump(isset($tar['big']));

$tar = new PharData(__DIR__ . '/gh25053-3.tar');
try {
    $tar->buildFromIterator(new ArrayIterator(['big' => fopen($src, 'rb')]));
} catch (BadMethodCallException $e) {
    echo $e->getMessage(), "\n";
}
var_dump(isset($tar['big']));

$tar = new PharData(__DIR__ . '/gh25053-iterator-retry.tar');
$small = fopen('php://temp', 'w+b');
fwrite($small, 'small');
rewind($small);
try {
    $tar->buildFromIterator(new ArrayIterator(['small' => $small, 'big' => $src]));
} catch (BadMethodCallException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($tar['small']->getContent());
$next = fopen('php://temp', 'w+b');
fwrite($next, 'next');
rewind($next);
$tar->buildFromIterator(new ArrayIterator(['next' => $next]));
var_dump($tar['small']->getContent());
var_dump($tar['next']->getContent());

$tar = new PharData(__DIR__ . '/gh25053-iterator-read-retry.tar');
$first = fopen('php://temp', 'w+b');
fwrite($first, 'first');
rewind($first);
$second = fopen('php://temp', 'w+b');
fwrite($second, 'second');
rewind($second);
try {
    $tar->buildFromIterator(new ArrayIterator(['first' => $first, 'second' => $second, 'big' => $src]));
} catch (BadMethodCallException $e) {
    echo $e->getMessage(), "\n";
}
$third = fopen('php://temp', 'w+b');
fwrite($third, 'third');
rewind($third);
$iterator = (function () use ($tar, $third) {
    var_dump($tar['first']->getContent());
    yield 'third' => $third;
})();
$tar->buildFromIterator($iterator);
var_dump($tar['first']->getContent());
var_dump($tar['second']->getContent());
var_dump($tar['third']->getContent());

$phar = new Phar(__DIR__ . '/gh25053-4.phar');
$phar->addFromString('small', 'x');
var_dump(copy($src, 'phar://' . __DIR__ . '/gh25053-4.phar/big'));

try {
    Phar::mount('phar://' . __DIR__ . '/gh25053-4.phar/big', $src);
} catch (PharException $e) {
    echo "mount failed\n";
}

$tar = new PharData(__DIR__ . '/gh25053-5.tar');
$fp = fopen($src, 'rb');
fseek($fp, 2**32);
$tar->buildFromIterator(new ArrayIterator(['tail' => $fp]));
var_dump($tar['tail']->getSize());

$tar = new PharData(__DIR__ . '/gh25053-6.tar');
$fp = fopen($src, 'rb');
fseek($fp, 102);
try {
    $tar['big'] = $fp;
} catch (BadMethodCallException $e) {
    echo $e->getMessage(), "\n";
}
$tar->addFromString('big', 'small');
var_dump($tar['big']->getContent());

$zip = new PharData(__DIR__ . '/gh25053-7.zip');
$zip->addFromString('keep', 'old');
$fp = fopen($src, 'rb');
fseek($fp, 12);
try {
    $zip['big'] = $fp;
} catch (PharException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($zip['keep']->getContent());
$zip->addFromString('big', 'small');
var_dump($zip['keep']->getContent());
var_dump($zip['big']->getContent());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25053.big');
@unlink(__DIR__ . '/gh25053-1.tar');
@unlink(__DIR__ . '/gh25053-2.tar');
@unlink(__DIR__ . '/gh25053-3.tar');
@unlink(__DIR__ . '/gh25053-iterator-retry.tar');
@unlink(__DIR__ . '/gh25053-iterator-read-retry.tar');
@unlink(__DIR__ . '/gh25053-4.phar');
@unlink(__DIR__ . '/gh25053-5.tar');
@unlink(__DIR__ . '/gh25053-6.tar');
@unlink(__DIR__ . '/gh25053-7.zip');
?>
--EXPECTF--
Entry big is too large to be stored in a phar archive
bool(false)
Entry big is too large to be stored in a phar archive
bool(false)
Entry big is too large to be stored in a phar archive
bool(false)
Entry big is too large to be stored in a phar archive
string(5) "small"
string(5) "small"
string(4) "next"
Entry big is too large to be stored in a phar archive
string(5) "first"
string(5) "first"
string(6) "second"
string(5) "third"

Warning: copy(): phar error: Could not write %d characters to "big" in phar "%sgh25053-4.phar", entry is too large in %s on line %d
bool(false)
mount failed
int(1)
Entry big is too large to be stored in a phar archive
string(5) "small"
phar zip flush of "%sgh25053-7.zip" failed: archive is too large for zip file format
string(3) "old"
string(3) "old"
string(5) "small"
