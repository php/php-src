--TEST--
Bug #81420 (ZipArchive::extractTo extracts outside of destination)
--SKIPIF--
<?php
if (!extension_loaded("zip")) die("skip zip extension not available");
?>
--FILE--
<?php
$zip = new ZipArchive();
$zip->open(__DIR__ . "/bug81420.zip");
$destination = __DIR__ . "/bug81420";
mkdir($destination);
$zip->extractTo($destination);
var_dump(file_exists("$destination/nt1/zzr_noharm.php"));
?>
--CLEAN--
<?php
$destination = __DIR__ . "/bug81420";
@unlink("$destination/nt1/zzr_noharm.php");
@rmdir("$destination/nt1");
@rmdir($destination);
?>
--EXPECT--
bool(true)
