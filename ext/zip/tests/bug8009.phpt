--TEST--
Bug #8009 (cannot add again same entry to an archive)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$thisdir = __DIR__;
$src = $thisdir . "/bug8009.zip";
$filename = $thisdir . "/tmp8009.zip";
copy($src, $filename);

$zip = new ZipArchive();

if (!$zip->open($filename)) {
       exit("cannot open $filename\n");
}
$zip->addFromString("2.txt", "=)");
$zip->close();
unlink($filename);
echo "status: " . $zip->status . "\n";
echo "\n";
?>
--EXPECT--
status: 0
