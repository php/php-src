--TEST--
ziparchive::properties isset()/empty() checks
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$dirname = dirname(__FILE__) . '/';
$file = $dirname . '__property_test.zip';

copy($dirname . 'test_with_comment.zip', $file);

$zip = new ZipArchive;
if (!$zip->open($file)) {
	exit('failed');
}

printf("zip->status (%d):\n\tempty(): %d\n\tisset(): %d\n", $zip->status, empty($zip->status), isset($zip->status));
printf("zip->numFiles (%d):\n\tempty(): %d\n\tisset(): %d\n", $zip->numFiles, empty($zip->numFiles), isset($zip->numFiles));
printf("zip->bogus (%d):\n\tempty(): %d\n\tisset(): %d\n", $zip->bogus, empty($zip->bogus), isset($zip->bogus));


$zip->addEmptyDir('emptydir');

printf("zip->status (%d):\n\tempty(): %d\n\tisset(): %d\n", $zip->status, empty($zip->status), isset($zip->status));
printf("zip->numFiles (%d):\n\tempty(): %d\n\tisset(): %d\n", $zip->numFiles, empty($zip->numFiles), isset($zip->numFiles));
printf("zip->filename (%d):\n\tempty(): %d\n\tisset(): %d\n", strlen($zip->filename), empty($zip->filename), isset($zip->filename));
printf("zip->comment (%d):\n\tempty(): %d\n\tisset(): %d\n", strlen($zip->comment), empty($zip->comment), isset($zip->comment));

unset($zip); //close the file before unlinking
@unlink($file);
?>
--EXPECTF--
zip->status (0):
	empty(): 1
	isset(): 1
zip->numFiles (4):
	empty(): 0
	isset(): 1

Notice: Undefined property: ZipArchive::$bogus in %s on line %d
zip->bogus (0):
	empty(): 1
	isset(): 0
zip->status (0):
	empty(): 1
	isset(): 1
zip->numFiles (5):
	empty(): 0
	isset(): 1
zip->filename (%d):
	empty(): 0
	isset(): 1
zip->comment (19):
	empty(): 0
	isset(): 1
