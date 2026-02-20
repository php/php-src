--TEST--
ZipArchive::openString leak tests
--EXTENSIONS--
zip
--FILE--
<?php
$zip = new ZipArchive;
$zip->openString(file_get_contents(__DIR__ . '/test.zip'));
$zip = null;

$zip = new ZipArchive;
$zip->openString(file_get_contents(__DIR__ . '/test.zip'));
$zip->openString(file_get_contents(__DIR__ . '/test.zip'));
$zip = null;

$zip = new ZipArchive;
$zip->openString(file_get_contents(__DIR__ . '/test.zip'));
$zip->open(__DIR__ . '/test.zip');
$zip = null;

$zip = new ZipArchive;
$zip->open(__DIR__ . '/test.zip');
$zip->openString(file_get_contents(__DIR__ . '/test.zip'));
$zip = null;
--EXPECT--
