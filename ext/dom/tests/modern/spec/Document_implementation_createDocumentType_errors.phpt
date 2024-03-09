--TEST--
Document::$implementation createDocumentType errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();
try {
    $dom->implementation->createDocumentType("invalid name", "public", "system");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $dom->implementation->createDocumentType("", "public", "system");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $dom->implementation->createDocumentType("@", "", "");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Namespace Error
Namespace Error
Namespace Error
