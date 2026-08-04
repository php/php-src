--TEST--
GH-22176 (Memory leak when a ZipArchive progress callback bails out in the shutdown destructor)
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!method_exists('ZipArchive', 'registerProgressCallback')) die('skip libzip too old');
?>
--FILE--
<?php
$zip = new ZipArchive;
$zip->open(__DIR__ . '/gh22176_progress.zip', ZipArchive::CREATE);
$zip->registerProgressCallback(0.5, function ($r) {
    throw new \Exception('progress boom');
});
$zip->addFromString('test', 'test');
echo "done\n";
// The archive is flushed and the object destroyed during request shutdown;
// the thrown exception bails out through libzip's zip_close() without leaking
// its internal state, and the bailout resumes once libzip has unwound.
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh22176_progress.zip');
?>
--EXPECTF--
done

Fatal error: Uncaught Exception: progress boom in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(0.0)
#1 {main}
  thrown in %s on line %d
