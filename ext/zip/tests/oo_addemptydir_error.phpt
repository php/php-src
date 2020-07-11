--TEST--
ziparchive::addEmptyDir error
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

include __DIR__ . '/utils.inc';

$zip = new ZipArchive;
if (!$zip->open(__DIR__ . '/test.zip', ZipArchive::RDONLY)) {
    exit('failed');
}

var_dump($zip->addEmptyDir('emptydir'));
if ($zip->status == ZipArchive::ER_RDONLY) {
    echo "OK\n";
} else if ($zip->status == ZipArchive::ER_OK) {
    dump_entries_name($zip);
} else {
    echo "Lost\n";
}
$zip->close();
?>
--EXPECT--
bool(false)
OK
