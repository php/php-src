--TEST--
Bug #40228 (extractTo does not create recursive empty path)
--SKIPIF--
<?php if (!extension_loaded("zip")) print "skip"; ?>
--FILE--
<?php
$dest = dirname(__FILE__);
$arc_name = $dest . "/bug40228私はガラスを食べられます.zip";
$zip = new ZipArchive;
$zip->open($arc_name, ZIPARCHIVE::CREATE);
$zip->extractTo($dest);
if (is_dir($dest . '/test/empty')) {
	echo "Ok\n";
	rmdir($dest . '/test/empty');
	rmdir($dest . '/test');
} else {
	echo "Failed.\n";
}
echo "Done\n";
?>
--EXPECT--
Ok
Done
