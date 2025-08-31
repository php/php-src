<?php
error_reporting(E_ALL);
if (!extension_loaded('zip')) {
    dl('zip.so');
}

$zip = new ZipArchive();
$filename = "a.zip";

if (!$zip->open($filename, ZIPARCHIVE::CREATE | ZipArchive::OVERWRITE)) {
    exit("cannot open <$filename>\n");
}

$zip->addFromString("testfilephp.txt", "#1 This is a test string added as testfilephp.txt.\n");
$zip->addFromString("testfilephp2.txt", "#2 This is a test string added as testfilephp2.txt.\n");
$zip->addFile("too.php", "testfromfile.php");

$zip->setCompressionName("testfilephp2.txt", ZipArchive::CM_STORE);
$zip->setCompressionIndex(2, ZipArchive::CM_STORE);

$zip->close();
