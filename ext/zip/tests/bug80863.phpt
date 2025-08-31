--TEST--
Bug #80863 (ZipArchive::extractTo() ignores references)
--EXTENSIONS--
zip
--FILE--
<?php
$archive = __DIR__ . "/bug80863.zip";

$zip = new ZipArchive();
$zip->open($archive, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addFromString("file1.txt", "contents");
$zip->addFromString("file2.txt", "contents");
$zip->close();

$target = __DIR__ . "/bug80683";
mkdir($target);

$files = [
    "file1.txt",
    "file2.txt",
];
// turn into references
foreach ($files as &$file);

$zip = new ZipArchive();
$zip->open($archive);
$zip->extractTo($target, $files);
var_dump(is_file("$target/file1.txt"));
var_dump(is_file("$target/file2.txt"));
?>
--EXPECT--
bool(true)
bool(true)
--CLEAN--
<?php
@unlink(__DIR__ . "/bug80863.zip");
$target = __DIR__ .  "/bug80683";
@unlink("$target/file1.txt");
@unlink("$target/file2.txt");
@rmdir($target);
?>
