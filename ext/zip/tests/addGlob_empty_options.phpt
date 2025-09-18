--TEST--
addGlob with empty options
--EXTENSIONS--
zip
--FILE--
<?php

touch($file = __DIR__ . '/addglob_empty_options.zip');

$zip = new ZipArchive();
$zip->open($file, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addGlob(__FILE__, 0, []);
var_dump($zip->statIndex(0)['name'] === __FILE__);
$zip->close();

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/addglob_empty_options.zip');
?>
--EXPECT--
bool(true)
