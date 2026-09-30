--TEST--
GH-23899 (Bailout during cancel callback return value conversion at shutdown)
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!method_exists(ZipArchive::class, 'registerCancelCallback')) {
    die('skip cancel callbacks are not supported');
}
?>
--INI--
zend.exception_ignore_args=1
--FILE--
<?php
set_error_handler(function ($errno, $message) {
    throw new Exception($message);
});

$zip = new ZipArchive;
$zip->open(__DIR__ . '/gh23899_conversion.zip', ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->registerCancelCallback(function () {
    return '123abc';
});
$zip->addFromString('test', 'test');
echo "Done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23899_conversion.zip');
?>
--EXPECTF--
Done

Fatal error: Uncaught Exception: A non-numeric value encountered in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}()
#1 {main}
  thrown in %s on line %d
