--TEST--
GH-23899 (Invalid cancel callback return type during shutdown causes an assertion failure)
--EXTENSIONS--
zip
--SKIPIF--
<?php
if (!method_exists(ZipArchive::class, 'registerCancelCallback')) {
    die('skip cancel callbacks are not supported');
}
?>
--FILE--
<?php
$zip = new ZipArchive;
$zip->open(__DIR__ . '/gh23899.zip', ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->registerCancelCallback(function () {
    return [new stdClass];
});
$zip->addFromString('test', 'test');
echo "Done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23899.zip');
?>
--EXPECTF--
Done

Fatal error: Uncaught TypeError: Return value of callback provided to ZipArchive::registerCancelCallback() must be of type int, array returned in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
