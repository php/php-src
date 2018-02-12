--TEST--
CURLFile methods errors
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
$file = new CurlFile(__DIR__ . '/curl_testdata1.txt');
var_dump($file->getMimeType(true));
var_dump($file->getFilename('file'));
var_dump($file->getPostFilename([]));
?>
--EXPECTF--
Warning: CURLFile::getMimeType() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: CURLFile::getFilename() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: CURLFile::getPostFilename() expects exactly 0 parameters, 1 given in %s on line %d
NULL
