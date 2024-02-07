--TEST--
DOM\HTMLDocument test values for encoding field
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
var_dump($dom->charset);
$dom->inputEncoding = "CSeuckr";
var_dump($dom->characterSet);
try {
    $dom->charset = "nope";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->charset);
$dom->inputEncoding = "Windows-1251";
var_dump($dom->characterSet);
try {
    $dom->charset = "";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->inputEncoding);
echo $dom->saveHTML();

try {
    $dom = DOM\HTMLDocument::createEmpty("bogus");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(5) "UTF-8"
string(6) "EUC-KR"
Invalid document encoding
string(6) "EUC-KR"
string(12) "windows-1251"
Invalid document encoding
string(12) "windows-1251"
DOM\HTMLDocument::createEmpty(): Argument #1 ($encoding) must be a valid document encoding
