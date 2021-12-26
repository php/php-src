--TEST--
Bug #40228 (extractTo does not create recursive empty path)
--SKIPIF--
<?php if (!extension_loaded("zip")) print "skip"; ?>
--FILE--
<?php
$dest = __DIR__ . "/bug40228-mb";
$arc_name = __DIR__ . "/bug40228私はガラスを食べられます.zip";
$zip = new ZipArchive;
$zip->open($arc_name, ZIPARCHIVE::CREATE);
$zip->extractTo($dest);
if (is_dir($dest . '/test/empty')) {
    echo "Ok\n";
    rmdir($dest . '/test/empty');
    rmdir($dest . '/test');
    rmdir($dest);
} else {
    echo "Failed.\n";
}
echo "Done\n";
?>
--EXPECT--
Ok
Done
