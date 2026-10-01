--TEST--
GH-21698 (ZipArchive::addGlob memory leak when open_basedir rejects the match)
--EXTENSIONS--
zip
--FILE--
<?php
$zipfile = __DIR__ . '/gh21698.zip';
$zip = new ZipArchive();
$zip->open($zipfile, ZipArchive::CREATE | ZipArchive::OVERWRITE);

ini_set('open_basedir', '/nonexistent_dir_for_gh21698');
var_dump($zip->addGlob(__FILE__, 0, []));
$zip->close();
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh21698.zip');
?>
--EXPECTF--
Warning: ZipArchive::addGlob(): open_basedir restriction in effect. File(%s) is not within the allowed path(s): (%s) in %s on line %d
bool(false)
