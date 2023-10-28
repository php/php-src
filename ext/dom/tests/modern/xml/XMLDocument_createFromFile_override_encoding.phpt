--TEST--
DOM\XMLDocument::createFromFile() with overrideEncoding
--EXTENSIONS--
dom
--FILE--
<?php

try {
    DOM\XMLDocument::createFromFile(__DIR__ . '/dummy.xml', overrideEncoding: 'nonexistent');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$dom = DOM\XMLDocument::createFromFile(__DIR__ . '/dummy.xml', overrideEncoding: 'UTF-8');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->encoding);

$dom = DOM\XMLDocument::createFromFile(__DIR__ . '/dummy.xml', overrideEncoding: 'Windows-1252');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->encoding);

?>
--EXPECT--
DOM\XMLDocument::createFromFile(): Argument #3 ($overrideEncoding) must be a valid document encoding
string(2) "é"
NULL
string(4) "Ã©"
NULL
