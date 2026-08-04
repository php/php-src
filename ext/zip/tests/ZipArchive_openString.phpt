--TEST--
ZipArchive::openString() method
--EXTENSIONS--
zip
--FILE--
<?php
echo "1. Open read-only and read directory\n";
$input = file_get_contents(__DIR__."/test_procedural.zip");
$zip = new ZipArchive();
$zip->openString($input, ZipArchive::RDONLY);

for ($i = 0; $i < $zip->numFiles; $i++) {
    $stat = $zip->statIndex($i);
    echo $stat['name'] . "\n";
}

// Zip is read-only, not allowed
var_dump($zip->addFromString("foobar/baz", "baz"));
var_dump($zip->addEmptyDir("blub"));

var_dump($zip->close());

echo "2. CREATE and EXCL flags\n";
$zip = new ZipArchive();
var_dump($zip->openString($input, ZipArchive::CREATE));
var_dump($zip->openString($input, ZipArchive::EXCL));
echo $zip->getStatusString() . "\n";

echo "3. CHECKCONS flag\n";
$inconsistent = file_get_contents(__DIR__ . '/checkcons.zip');
$zip = new ZipArchive();
var_dump($zip->openString($inconsistent));
var_dump($zip->openString($inconsistent, ZipArchive::CHECKCONS));

?>
--EXPECTF--
1. Open read-only and read directory
foo
bar
foobar/
foobar/baz
bool(false)
bool(false)
bool(true)
2. CREATE and EXCL flags
bool(true)
int(10)
File already exists
3. CHECKCONS flag
bool(true)
int(%d)
