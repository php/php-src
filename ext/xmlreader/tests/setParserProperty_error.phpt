--TEST--
XMLReader: setParserProperty Error
--EXTENSIONS--
xmlreader
--FILE--
<?php

$xml = '<?xml version="1.0" encoding="UTF-8"?><books><book>new book</book></books>';

$invalidProperty = -1;
$reader = new XMLReader();
$reader->XML($xml);
try {
    $reader->setParserProperty(-1, true);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
$reader->close();
?>
--EXPECT--
XMLReader::setParserProperty(): Argument #1 ($property) must be a valid parser property
