--TEST--
DOM\HTMLDocument::createFromString() with override_encoding
--EXTENSIONS--
dom
--FILE--
<?php

try {
    DOM\HTMLDocument::createFromString(file_get_contents(__DIR__ . '/gb18030_without_charset.html'), override_encoding: 'nonexistent');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

$dom = DOM\HTMLDocument::createFromString(file_get_contents(__DIR__ . '/gb18030_without_charset.html'), override_encoding: 'GB18030');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->encoding);

$dom = DOM\HTMLDocument::createFromString(file_get_contents(__DIR__ . '/fallback_encoding.html'), override_encoding: 'Windows-1252');
var_dump($dom->documentElement->lastChild->textContent);
var_dump($dom->encoding);

?>
--EXPECT--
DOM\HTMLDocument::createFromString(): Argument #3 ($override_encoding) must be a valid document encoding
string(20) "
    Héllo, world!
"
string(7) "gb18030"
string(1) "
"
string(12) "windows-1252"
