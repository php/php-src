--TEST--
Bug GH-8781 (ZipArchive deletes zip file with no contents)
--EXTENSIONS--
zip
--FILE--
<?php
touch($file = __DIR__ . '/bug_gh8781.zip');
var_dump(is_file($file));

$zip = new ZipArchive();
$zip->open($file, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->close();

var_dump(is_file($file));
?>
--EXPECT--
bool(true)
bool(false)

