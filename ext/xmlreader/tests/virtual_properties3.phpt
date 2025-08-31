--TEST--
Virtual property unset tests
--EXTENSIONS--
xmlreader
--FILE--
<?php

class MyXMLReader extends XMLReader
{
    public int $x;
}

$reader = new MyXMLReader();

try {
    unset($reader->attributeCount);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    unset($reader->baseURI);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    unset($reader->depth);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    unset($reader->hasAttributes);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    unset($reader->hasValue);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

unset($reader->x);
var_dump(isset($reader->x));

?>
--EXPECT--
Cannot unset MyXMLReader::$attributeCount
Cannot unset MyXMLReader::$baseURI
Cannot unset MyXMLReader::$depth
Cannot unset MyXMLReader::$hasAttributes
Cannot unset MyXMLReader::$hasValue
bool(false)
