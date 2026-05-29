--TEST--
GH-22176 (Memory leak when a ZipArchive cancel callback bails out in the shutdown destructor)
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!method_exists('ZipArchive', 'registerCancelCallback')) die('skip libzip too old');
?>
--FILE--
<?php
$zip = new ZipArchive;
$zip->open(__DIR__ . '/gh22176_cancel.zip', ZipArchive::CREATE);
$zip->registerCancelCallback(function () {
    throw new \Exception('cancel boom');
});
$zip->addFromString('test', 'test');
echo "done\n";
// The archive is flushed and the object destroyed during request shutdown;
// the thrown exception bails out through libzip's zip_close() without leaking
// its internal state, and the bailout resumes once libzip has unwound.
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh22176_cancel.zip');
?>
--EXPECTF--
done

Fatal error: Uncaught Exception: cancel boom in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}()
#1 {main}
  thrown in %s on line %d
