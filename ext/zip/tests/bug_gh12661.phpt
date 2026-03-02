--TEST--
Bug GH-12661 (Inconsistency in ZipArchive::addGlob 'remove_path' Option Behavior)
--EXTENSIONS--
zip
--FILE--
<?php
include __DIR__ . '/utils.inc';

touch($file = __DIR__ . '/bug_gh12661.zip');

$zip = new ZipArchive();
$zip->open($file, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$zip->addGlob(__FILE__, 0, ['remove_path' => 'bug_']);  // unchanged (bug is not a prefix)
$zip->addGlob(__FILE__, 0, ['remove_path' => dirname(__DIR__)]);
verify_entries($zip, [__FILE__, basename(__DIR__) . DIRECTORY_SEPARATOR . basename(__FILE__)]);
$zip->close();

?>
Done
--CLEAN--
<?php
unlink(__DIR__ . '/bug_gh12661.zip');
?>
--EXPECT--
Done
