--TEST--
Bug #78641 (addGlob can modify given remove_path value)
--SKIPIF--
<?php
if (!extension_loaded('zip')) die('skip zip extension not available');
?>
--FILE--
<?php
define("TMPDIR", __DIR__ . "/");

$file = TMPDIR . 'bug78641';
touch($file);

$zip = new ZipArchive();
$zip->open(TMPDIR . "bug78641.zip", ZipArchive::CREATE | ZipArchive::OVERWRITE);
var_dump(basename(TMPDIR));
$zip->addGlob($file, 0, ["remove_path" => TMPDIR]);
var_dump(basename(TMPDIR));
$zip->close();
?>
--EXPECT--
string(5) "tests"
string(5) "tests"
--CLEAN--
<?php
unlink(__DIR__ . '/bug78641');
unlink(__DIR__ . '/bug78641.zip');
?>
