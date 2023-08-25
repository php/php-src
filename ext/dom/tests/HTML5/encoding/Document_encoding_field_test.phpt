--TEST--
DOM\HTML5Document test values for encoding field
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
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

?>
--EXPECT--
NULL
string(6) "EUC-KR"
Invalid document encoding
string(6) "EUC-KR"
string(12) "windows-1251"
Invalid document encoding
string(12) "windows-1251"
