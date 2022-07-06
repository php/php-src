--TEST--
XMLReader: libxml2 XML Reader, file data
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
--FILE--
<?php
$filename = __DIR__ . '/_002.xml';
$xmlstring = '<?xml version="1.0" encoding="UTF-8"?>
<books></books>';
file_put_contents($filename, $xmlstring);

$reader = new XMLReader();
try {
    $reader->open('');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

$reader = new XMLReader();
if (!$reader->open($filename)) {
    $reader->close();
    exit();
}

// Only go through
while ($reader->read()) {
    echo $reader->name."\n";
}
$reader->close();
unlink($filename);
touch($filename);
$reader = new XMLReader();
$reader->open($filename);
$reader->close();
unlink($filename);

?>
--EXPECT--
XMLReader::open(): Argument #1 ($uri) cannot be empty
books
books
