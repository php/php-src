--TEST--
Bug #40228 (extractTo does not create recursive empty path)
--EXTENSIONS--
zip
--FILE--
<?php
$dest = __DIR__ . "/bug40228";
$arc_name = __DIR__ . "/bug40228.zip";
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
