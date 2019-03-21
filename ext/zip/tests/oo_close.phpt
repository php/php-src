--TEST--
zip::close() function
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$dirname = __DIR__ . '/';
$zip = new ZipArchive;
if (!$zip->open($dirname . 'test.zip')) {
	exit('failed');
}

if ($zip->status == ZIPARCHIVE::ER_OK) {
	$zip->close();
	echo "ok\n";
} else {
	echo "failed\n";
}
?>
--EXPECT--
ok
