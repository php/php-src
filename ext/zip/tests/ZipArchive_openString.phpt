--TEST--
ZipArchive::openString() method
--EXTENSIONS--
zip
--FILE--
<?php
echo "1.\n";
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

echo "2.\n";
$zip = new ZipArchive();
var_dump($zip->openString($input, ZipArchive::CREATE));
var_dump($zip->openString($input, ZipArchive::EXCL));
echo $zip->getStatusString() . "\n";

echo "3.\n";
$inconsistent = file_get_contents(__DIR__ . '/checkcons.zip');
$zip = new ZipArchive();
var_dump($zip->openString($inconsistent));
var_dump($zip->openString($inconsistent, ZipArchive::CHECKCONS));

?>
--EXPECTF--
1.
foo
bar
foobar/
foobar/baz
bool(false)
bool(false)
bool(true)
2.
bool(true)
int(10)
File already exists
3.
bool(true)
int(%d)
