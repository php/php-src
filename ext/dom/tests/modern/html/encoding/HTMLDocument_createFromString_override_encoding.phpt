--TEST--
DOM\HTMLDocument::createFromString() with overrideEncoding
--EXTENSIONS--
dom
--FILE--
<?php

try {
    DOM\HTMLDocument::createFromString(file_get_contents(__DIR__ . '/gb18030_without_charset.html'), overrideEncoding: 'nonexistent');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$dom = DOM\HTMLDocument::createFromString(file_get_contents(__DIR__ . '/gb18030_without_charset.html'), overrideEncoding: 'GB18030');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->encoding);

$dom = DOM\HTMLDocument::createFromString(file_get_contents(__DIR__ . '/fallback_encoding.html'), overrideEncoding: 'Windows-1252');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->encoding);

?>
--EXPECT--
DOM\HTMLDocument::createFromString(): Argument #3 ($overrideEncoding) must be a valid document encoding
string(20) "
    Héllo, world!
"
string(7) "gb18030"
string(1) "
"
string(12) "windows-1252"
