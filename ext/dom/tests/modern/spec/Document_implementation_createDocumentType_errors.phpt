--TEST--
Document::$implementation createDocumentType errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();
try {
    $dom->implementation->createDocumentType("invalid name", "public", "system");
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $dom->implementation->createDocumentType("", "public", "system");
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $dom->implementation->createDocumentType("@", "", "");
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Namespace Error
DOMException: Namespace Error
DOMException: Namespace Error
