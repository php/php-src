--TEST--
Document::$implementation createDocumentType errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();
try {
    $dom->implementation->createDocumentType("invalid name", "public", "system");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $dom->implementation->createDocumentType("", "public", "system");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $dom->implementation->createDocumentType("@", "", "");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Namespace Error
DOMException: Namespace Error
DOMException: Namespace Error
