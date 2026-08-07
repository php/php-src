--TEST--
Dom\HTMLDocument::createCDATASection()
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createEmpty();
try {
    $dom->createCDATASection("foo");
} catch (DOMException $e) {
    var_dump($e->getCode());
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $dom->createCDATASection("]]>");
} catch (DOMException $e) {
    var_dump($e->getCode());
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$dom = Dom\XMLDocument::createEmpty();
try {
    $dom->createCDATASection("]]>");
} catch (DOMException $e) {
    var_dump($e->getCode());
    echo $e::class, ': ', $e->getMessage(), "\n";
}
$dom->createCDATASection("]>");
?>
--EXPECT--
int(9)
DOMException: This operation is not supported for HTML documents
int(9)
DOMException: This operation is not supported for HTML documents
int(5)
DOMException: Invalid character sequence "]]>" in CDATA section
