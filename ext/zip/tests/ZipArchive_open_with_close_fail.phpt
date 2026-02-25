--TEST--
zip_close() failure from ZipArchive::open()
--EXTENSIONS--
zip
--FILE--
<?php
// Try to create an archive
$zip = new ZipArchive();
var_dump($zip->open(__DIR__ . '/close-fail.zip', ZipArchive::CREATE));
file_put_contents(__DIR__.'/close-fail-file', '');
$zip->addFile(__DIR__.'/close-fail-file');
// Delete the file to trigger an error on close
@unlink(__DIR__.'/close-fail-file');
var_dump($zip->open(__DIR__ . '/test.zip', ZipArchive::RDONLY));
var_dump($zip->getFromName('bar'));
?>
--EXPECTF--
bool(true)

Warning: ZipArchive::open(): Can't open file: No such file or directory in %s on line %d
bool(true)
string(4) "bar
"
