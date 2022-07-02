--TEST--
XMLReader: setSchema Error
--EXTENSIONS--
xmlreader
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

$reader = new XMLReader();
$reader->XML(<<<EOF
<?xml version="1.0" encoding="UTF-8" ?>
<foo/>
EOF);
var_dump(@$reader->setSchema('schema-bad.xsd'));
$reader->close();
?>
--EXPECT--
XMLReader::setSchema(): Argument #1 ($filename) cannot be empty
Schema must be set prior to reading
Schema must be set prior to reading
bool(false)
