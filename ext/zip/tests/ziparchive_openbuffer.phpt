--TEST--
ZipArchive::openBuffer() method
--EXTENSIONS--
zip
--FILE--
<?php
$zip = new ZipArchive();
$zip->openBuffer(file_get_contents(__DIR__."/test_procedural.zip"));

for ($i = 0; $i < $zip->numFiles; $i++) {
    $stat = $zip->statIndex($i);
    echo $stat['name'] . "\n";
}

// Zip is read-only, not allowed
var_dump($zip->addFromString("foobar/baz", "baz"));
var_dump($zip->addEmptyDir("blub"));

var_dump($zip->close());
?>
--EXPECTF--
foo
bar
foobar/
foobar/baz
bool(false)
bool(false)
bool(true)
