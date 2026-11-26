--TEST--
GH-25360 (Assertion failure when the error handler throws on the implicit close at shutdown)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
zip
--FILE--
<?php
$zip = new ZipArchive();
$zip->open(__DIR__ . '/gh25360.zip', ZipArchive::CREATE | ZipArchive::OVERWRITE);
file_put_contents(__DIR__ . '/gh25360.txt', 'contents');
$zip->addFile(__DIR__ . '/gh25360.txt', 'file.txt');
unlink(__DIR__ . '/gh25360.txt');
set_error_handler(function () {
    throw new Exception('handler');
});
echo "Done\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25360.zip');
@unlink(__DIR__ . '/gh25360.txt');
?>
--EXPECTF--
Done

Fatal error: Uncaught Exception: handler in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(2, '%s', 'Unknown', 0)
#1 {main}
  thrown in %s on line %d
