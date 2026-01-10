--TEST--
addGlob with too long add_path option
--EXTENSIONS--
zip
--FILE--
<?php

touch($file = __DIR__ . '/addglob_too_long_add_path.zip');

$zip = new ZipArchive();
$zip->open($file, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addGlob(__FILE__, 0, ['add_path' => str_repeat('A', PHP_MAXPATHLEN - 2)]);
$zip->close();

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/addglob_too_long_add_path.zip');
?>
--EXPECTF--
Warning: ZipArchive::addGlob(): Entry name too long (max: %d, %d given) in %s on line %d
