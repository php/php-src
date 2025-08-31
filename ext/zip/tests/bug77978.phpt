--TEST--
Bug #77978 (Dirname ending in colon unzips to wrong dir)
--EXTENSIONS--
zip
--FILE--
<?php
$file = __DIR__ . "/bug77978.zip";
$target = __DIR__ . "/bug77978";

mkdir($target);

$zip = new ZipArchive();
$zip->open($file, ZipArchive::CREATE|ZipArchive::OVERWRITE);
$zip->addFromString("dir/test:/filename.txt", "contents");
$zip->close();

$zip->open($file);
// Windows won't extract filenames with colons; we suppress the warning
@$zip->extractTo($target, "dir/test:/filename.txt");
$zip->close();

var_dump(!file_exists("$target/filename.txt"));
var_dump(PHP_OS_FAMILY === "Windows" || file_exists("$target/dir/test:/filename.txt"));
?>
--EXPECT--
bool(true)
bool(true)
--CLEAN--
<?php
@unlink(__DIR__ . "/bug77978.zip");
@unlink(__DIR__ . "/bug77978/dir/test:/filename.txt");
@rmdir(__DIR__ . "/bug77978/dir/test:");
@rmdir(__DIR__ . "/bug77978/dir");
@rmdir(__DIR__ . "/bug77978");
?>
