--TEST--
ZipArchive::registerCancelCallback() with a callback returning an incorrect type
--EXTENSIONS--
zip
--SKIPIF--
<?php
/* $Id$ */
if (!method_exists('ZipArchive', 'registerCancelCallback')) die('skip libzip too old');
?>
--INI--
date.timezone=UTC
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
$file = $dirname . '__tmp_oo_cancel_incorrect_return.zip';

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
    exit('failed');
}

/* Register a bogus callback */
var_dump($zip->registerCancelCallback(function () {
    return [];
}));
var_dump($zip->addFromString(PHP_BINARY, 'entry #1'));

var_dump($zip->close());
var_dump($zip->status == ZipArchive::ER_CANCELLED);
var_dump($zip->getStatusString());
@unlink($file);
?>
Done
--CLEAN--
<?php
$dirname = dirname(__FILE__) . '/';
$file = $dirname . '__tmp_oo_cancel_incorrect_return.zip';

@unlink($file);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
string(8) "No error"
Done
