--TEST--
Bug #71509 Zip problem with swedish letters in filename.
--SKIPIF--
<?php
include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

skip_if_no_required_exts("zip");
skip_if_not_win();

?>
--FILE--
<?PHP
// åöä

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";

$prefix = create_data("bug71509");
$testfile = $prefix . DIRECTORY_SEPARATOR . "Röd_Statistics.txt"; // cp1252
$outputfile = $prefix . DIRECTORY_SEPARATOR . "Röd_Statistics.zip"; // cp1252

$testfile_zip = iconv("ISO-8859-1", "UTF-8", $testfile);
$outputfile_zip = iconv("ISO-8859-1", "UTF-8", $outputfile);

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
===DONE===
--EXPECT--
bool(true)
bool(true)
===DONE===
