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

// The override encoding matches with the document encoding attribute
$dom = DOM\XMLDocument::createFromFile(__DIR__ . '/dummy.xml', overrideEncoding: 'UTF-8');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->charset);

// The override encoding mismatches with the document encoding attribute
$dom = DOM\XMLDocument::createFromFile(__DIR__ . '/dummy.xml', overrideEncoding: 'Windows-1252');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->charset);

?>
--EXPECT--
DOM\XMLDocument::createFromFile(): Argument #3 ($overrideEncoding) must be a valid document encoding
string(2) "é"
string(5) "UTF-8"
string(4) "Ã©"
string(12) "Windows-1252"
