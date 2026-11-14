--TEST--
GH-25053: Phar rejects mounts, tar entries and zip archives beyond the 32-bit format limits
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
$src = __DIR__ . '/gh25053_format_limits.big';
$f = fopen($src, 'w');
ftruncate($f, 2**32 + 1);
fclose($f);

$phar = new Phar(__DIR__ . '/gh25053_format_limits.phar');
$phar->addFromString('small', 'x');
try {
    Phar::mount('phar://' . __DIR__ . '/gh25053_format_limits.phar/big', $src);
} catch (PharException $e) {
    echo "mount failed\n";
}

$tar = new PharData(__DIR__ . '/gh25053_format_limits.tar');
$fp = fopen($src, 'rb');
fseek($fp, 102);
try {
    $tar['big'] = $fp;
} catch (PharException $e) {
    echo $e->getMessage(), "\n";
}

$zip = new PharData(__DIR__ . '/gh25053_format_limits.zip');
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
@unlink(__DIR__ . '/gh25053_format_limits.big');
@unlink(__DIR__ . '/gh25053_format_limits.phar');
@unlink(__DIR__ . '/gh25053_format_limits.tar');
@unlink(__DIR__ . '/gh25053_format_limits.zip');
?>
--EXPECTF--
mount failed
tar-based phar "%sgh25053_format_limits.tar" cannot be created, file "big" is too large for tar file format
phar zip flush of "%sgh25053_format_limits.zip" failed: archive is too large for zip file format
string(3) "old"
string(3) "old"
string(5) "small"
