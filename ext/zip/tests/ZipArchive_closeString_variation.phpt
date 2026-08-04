--TEST--
ZipArchive::closeString() variations
--EXTENSIONS--
zip
--FILE--
<?php
echo "1. Empty archive creation\n";
$zip = new ZipArchive();
$zip->openString();
var_dump($zip->closeString());
echo $zip->getStatusString() . "\n";

echo "2. Update existing archive\n";
$input = file_get_contents(__DIR__ . '/test.zip');
$zip = new ZipArchive();
$zip->openString($input);
$zip->addFromString('entry1.txt', '');
$result = $zip->closeString();
echo gettype($result) . "\n";
var_dump($input !== $result);

echo "3. Unchanged existing archive\n";
$zip = new ZipArchive();
$zip->openString($input);
$result = $zip->closeString();
echo gettype($result) . "\n";
var_dump($input === $result);

?>
--EXPECT--
1. Empty archive creation
string(0) ""
No error
2. Update existing archive
string
bool(true)
3. Unchanged existing archive
string
bool(true)
