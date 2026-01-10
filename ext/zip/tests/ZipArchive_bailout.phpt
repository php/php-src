--TEST--
ZipArchive destructor should be called on bailout
--EXTENSIONS--
zip
--FILE--
<?php

function &cb() {}

$file = __DIR__ . '/gh18907.zip';
$zip = new ZipArchive;
$zip->open($file, ZIPARCHIVE::CREATE);
$zip->registerCancelCallback(cb(...));
$zip->addFromString('test', 'test');
$fusion = $zip;

?>
--CLEAN--
<?php
$file = __DIR__ . '/gh18907.zip';
@unlink($file);
?>
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d

Notice: Only variable references should be returned by reference in %s on line %d

Notice: Only variable references should be returned by reference in %s on line %d
