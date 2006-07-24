<?php
$zip = new ZipArchive();

echo $zip->filename . "\n";
$zip->open("test.zip");
/*
$zip->addFile("./modules/");
$zip->addFile("./testempty");
*/
echo $zip->status . "\n";
echo $zip->statusSys . "\n";

echo $zip->numFiles . "\n";
echo $zip->filename . "\n";
var_dump($zip);
$files = array('test', 'testdir/test2');
if (!$zip->extractTo("./testext/path/to")) {
	echo "error!\n";
	echo $zip->status . "\n";
	echo $zip->statusSys . "\n";

}

$zip->close();
