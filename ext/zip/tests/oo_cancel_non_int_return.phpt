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
try {
    var_dump($zip->close());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
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
--EXPECTF--
bool(true)
bool(true)

Warning: ZipArchive::close(): Operation cancelled in %s on line %d
TypeError: Return value of callback provided to ZipArchive::registerCancelCallback() must be of type int, array returned
bool(true)
string(19) "Operation cancelled"
Done
