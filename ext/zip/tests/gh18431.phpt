--TEST--
GH-18431 (Registering ZIP progress callback twice doesn't work)
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!method_exists(ZipArchive::class, 'registerProgressCallback')) {
    die('skip progress callbacks are not supported');
}
?>
--FILE--
<?php
$file = __DIR__ . '/gh18431.zip';
$callback = var_dump(...);
$zip = new ZipArchive;
$zip->open($file, ZIPARCHIVE::CREATE);
$zip->registerProgressCallback(0.5, $callback);
$zip->registerProgressCallback(0.5, $callback);
$zip->addFromString('foo', 'entry #1');
?>
--CLEAN--
<?php
$file = __DIR__ . '/gh18431.zip';
@unlink($file);
?>
--EXPECT--
float(0)
float(1)
