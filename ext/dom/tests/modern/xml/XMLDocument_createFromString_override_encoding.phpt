--TEST--
DOM\XMLDocument::createFromString() with overrideEncoding
--EXTENSIONS--
dom
--FILE--
<?php

try {
    DOM\XMLDocument::createFromString(file_get_contents(__DIR__ . '/dummy.xml'), overrideEncoding: 'nonexistent');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// The override encoding matches with the document encoding attribute
$dom = DOM\XMLDocument::createFromString(file_get_contents(__DIR__ . '/dummy.xml'), overrideEncoding: 'UTF-8');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->encoding);

// The override encoding mismatches with the document encoding attribute
$dom = DOM\XMLDocument::createFromString(file_get_contents(__DIR__ . '/dummy.xml'), overrideEncoding: 'Windows-1252');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->encoding);

?>
--EXPECT--
DOM\XMLDocument::createFromString(): Argument #3 ($overrideEncoding) must be a valid document encoding
string(2) "é"
string(5) "UTF-8"
string(4) "Ã©"
string(12) "Windows-1252"
