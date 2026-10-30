--TEST--
ZipArchive::extractTo() with a non-list files array (non-sequential keys)
--EXTENSIONS--
zip
--FILE--
<?php
$archive = __DIR__ . "/oo_extract_array_keys.zip";

$zip = new ZipArchive();
$zip->open($archive, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString("file0.txt", "zero");
$zip->addFromString("file1.txt", "one");
$zip->close();

$target = __DIR__ . "/oo_extract_array_keys";
mkdir($target);

// array_filter() (and array_unique/array_diff) preserve keys, so this is [1 => "file1.txt"],
// a perfectly valid list of entry names that is not a packed 0-based array.
$files = array_filter(["file0.txt", "file1.txt"], fn($f) => $f === "file1.txt");

$zip = new ZipArchive();
$zip->open($archive);
var_dump($zip->extractTo($target, $files));
$zip->close();

var_dump(is_file("$target/file1.txt"));
?>
--EXPECT--
bool(true)
bool(true)
--CLEAN--
<?php
@unlink(__DIR__ . "/oo_extract_array_keys.zip");
$target = __DIR__ . "/oo_extract_array_keys";
@unlink("$target/file1.txt");
@rmdir($target);
?>
