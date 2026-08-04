--TEST--
ZipArchive::closeString() error cases
--EXTENSIONS--
zip
--FILE--
<?php
echo "1.\n";
$zip = new ZipArchive();
$zip->openString();
var_dump($zip->open(__DIR__ . '/test.zip'));
try {
	$zip->closeString();
} catch (Error $e) {
	echo $e->getMessage() . "\n";
}

echo "2.\n";
$zip = new ZipArchive();
$zip->openString('...');
echo $zip->getStatusString() . "\n";
try {
	$zip->closeString();
} catch (Error $e) {
	echo $e->getMessage() . "\n";
}

echo "3.\n";
$zip = new ZipArchive();
$zip->openString(file_get_contents(__DIR__ . '/test.zip'));
echo gettype($zip->closeString()) . "\n";
try {
	$zip->closeString();
} catch (Error $e) {
	echo $e->getMessage() . "\n";
}

?>
--EXPECT--
1.
bool(true)
ZipArchive::closeString can only be called on an archive opened with ZipArchive::openString
2.
Not a zip archive
Invalid or uninitialized Zip object
3.
string
Invalid or uninitialized Zip object
