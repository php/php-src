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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($reader->baseURI);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($reader->depth);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($reader->hasAttributes);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($reader->hasValue);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

unset($reader->x);
var_dump(isset($reader->x));

?>
--EXPECT--
Error: Cannot unset MyXMLReader::$attributeCount
Error: Cannot unset MyXMLReader::$baseURI
Error: Cannot unset MyXMLReader::$depth
Error: Cannot unset MyXMLReader::$hasAttributes
Error: Cannot unset MyXMLReader::$hasValue
bool(false)
