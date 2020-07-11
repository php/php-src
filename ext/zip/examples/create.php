<?php
error_reporting(E_ALL);
if (!extension_loaded('zip')) {
    dl('zip.so');
}
$thisdir = dirname(__FILE__);
unlink("./test112.zip");
$zip = new ZipArchive();
$filename = "./test112.zip";

if (!$zip->open($filename, ZIPARCHIVE::CREATE)) {
    exit("cannot open <$filename>\n");
} else {
    echo "file <$filename> OK\n";
}

$zip->addFromString("testfilephp.txt" . time(), "#1 This is a test string added as testfilephp.txt.\n");
$zip->addFromString("testfilephp2.txt" . time(), "#2 This is a test string added as testfilephp2.txt.\n");
$zip->addFile($thisdir . "/too.php","/testfromfile.php");
echo "numfiles: " . $zip->numFiles . "\n";
echo "status:" . $zip->status . "\n";
$zip->close();
unset($zip);
