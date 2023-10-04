--TEST--
DOM\XMLDocument::createFromString() with override_encoding
--EXTENSIONS--
dom
--FILE--
<?php

try {
    DOM\XMLDocument::createFromString(file_get_contents(__DIR__ . '/dummy.xml'), override_encoding: 'nonexistent');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$dom = DOM\XMLDocument::createFromString(file_get_contents(__DIR__ . '/dummy.xml'), override_encoding: 'UTF-8');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->encoding);

$dom = DOM\XMLDocument::createFromString(file_get_contents(__DIR__ . '/dummy.xml'), override_encoding: 'Windows-1252');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->encoding);

?>
--EXPECT--
DOM\XMLDocument::createFromString(): Argument #3 ($override_encoding) must be a valid document encoding
string(2) "é"
NULL
string(4) "Ã©"
NULL
