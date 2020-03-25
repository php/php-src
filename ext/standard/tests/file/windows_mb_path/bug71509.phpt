--TEST--
Bug #71509 Zip problem with swedish letters in filename.
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts("zip");

?>
--CONFLICTS--
bug71509
--FILE--
<?PHP
// åöä

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$item = iconv("ISO-8859-1", "UTF-8", "Röd_Statistics"); // cp1252
$prefix = create_data("bug71509", "$item.txt");
$testfile_zip = $prefix . DIRECTORY_SEPARATOR . "$item.txt";
$outputfile_zip = $prefix . DIRECTORY_SEPARATOR . "$item.zip";

var_dump(file_exists($testfile_zip));

$zipfile = new ZipArchive;

$return_code = $zipfile->open($outputfile_zip, ZipArchive::CREATE);
if ($return_code != true) die("Failed to open file: " . $return_code);

$return_code = $zipfile->addfile($testfile_zip, basename($testfile_zip));
if ($return_code != true) print("Failed to add file: " . $zipfile->getStatusString());

$return_code = $zipfile->close();
if ($return_code != true) die("Failed to close archive: " . $zipfile->getStatusString());

var_dump(file_exists($outputfile_zip));

remove_data("bug71509");
?>
--EXPECT--
bool(true)
bool(true)
