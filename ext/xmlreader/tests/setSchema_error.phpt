--TEST--
XMLReader: setSchema Error
--SKIPIF--
<?php if (!extension_loaded("xmlreader")) print "skip XMLReader extension required"; ?>
--FILE--
<?php

$reader = new XMLReader();
try {
    $reader->setSchema('');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
$reader->close();

$reader = new XMLReader();
try {
    $reader->setSchema('schema-missing-file.xsd');
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}
$reader->close();

$reader = new XMLReader();
try {
    $reader->setSchema('schema-empty.xsd');
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}
$reader->close();
?>
--EXPECT--
XMLReader::setSchema(): Argument #1 ($filename) cannot be empty
Unable to set schema. This must be set prior to reading or schema contains errors.
Unable to set schema. This must be set prior to reading or schema contains errors.
