--TEST--
GH-22176 (Memory leak when a ZipArchive cancel/progress callback bails out in the shutdown destructor)
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!method_exists('ZipArchive', 'registerCancelCallback')) die('skip libzip too old');
if (!method_exists('ZipArchive', 'registerProgressCallback')) die('skip libzip too old');
?>
--FILE--
<?php
$dir = __DIR__ . '/';

$cancel = new ZipArchive;
$cancel->open($dir . 'gh22176_cancel.zip', ZipArchive::CREATE);
$cancel->registerCancelCallback(function () {
    throw new \Exception('cancel boom');
});
$cancel->addFromString('test', 'test');

$progress = new ZipArchive;
$progress->open($dir . 'gh22176_progress.zip', ZipArchive::CREATE);
$progress->registerProgressCallback(0.5, function ($r) {
    throw new \Exception('progress boom');
});
$progress->addFromString('test', 'test');

echo "done\n";
// Both archives are flushed and the objects destroyed during request
// shutdown; the thrown exceptions bail out through libzip's zip_close()
// without leaking its internal state.
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh22176_cancel.zip');
@unlink(__DIR__ . '/gh22176_progress.zip');
?>
--EXPECTF--
done

Fatal error: Uncaught Exception: progress boom in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(0.0)
#1 {main}
  thrown in %s on line %d

Fatal error: Uncaught Exception: progress boom in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(1.0)
#1 {main}
  thrown in %s on line %d

Fatal error: Uncaught Exception: cancel boom in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}()
#1 {main}
  thrown in %s on line %d

Warning: PHP Request Shutdown: Cannot destroy the zip context: %s in Unknown on line 0
