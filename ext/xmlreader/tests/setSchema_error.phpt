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
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
$reader->close();

$reader = new XMLReader();
try {
    $reader->setSchema('schema-missing-file.xsd');
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
$reader->close();

$reader = new XMLReader();
try {
    $reader->setSchema('schema-empty.xsd');
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
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
ValueError: XMLReader::setSchema(): Argument #1 ($filename) must not be empty
Error: Schema must be set prior to reading
Error: Schema must be set prior to reading
bool(false)
