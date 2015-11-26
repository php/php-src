--TEST--
zip::open() function
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$dirname = dirname(__FILE__) . '/';
$zip = new ZipArchive;
$r = $zip->open($dirname . 'nofile');
if ($r !== TRUE) {
	echo "ER_OPEN: ok\n";
} else {
	echo "ER_OPEN: FAILED\n";
}

$r = $zip->open($dirname . 'nofile', ZIPARCHIVE::CREATE);
if (!$r) {
	echo "create: failed\n";
} else {
	echo "create: ok\n";
}
@unlink($dirname . 'nofile');

$zip = new ZipArchive;
$zip->open('');

if (!$zip->open($dirname . 'test.zip')) {
	exit("failed 1\n");
}

if ($zip->status == ZIPARCHIVE::ER_OK) {
	echo "OK\n";
} else {
	echo "failed\n";
}
?>
--EXPECTF--
ER_OPEN: ok
create: ok

Warning: ZipArchive::open(): Empty string as source in %s on line %d
OK
