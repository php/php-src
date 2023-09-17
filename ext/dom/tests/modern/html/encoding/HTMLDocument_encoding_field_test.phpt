--TEST--
DOM\HTMLDocument test values for encoding field
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
var_dump($dom->encoding);
$dom->encoding = "CSeuckr";
var_dump($dom->encoding);
try {
    $dom->encoding = "nope";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->encoding);
$dom->encoding = "Windows-1251";
var_dump($dom->encoding);
try {
    $dom->encoding = NULL;
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($dom->encoding);
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
DOM\HTMLDocument::createEmpty(): Argument #1 ($encoding) is not a valid document encoding
