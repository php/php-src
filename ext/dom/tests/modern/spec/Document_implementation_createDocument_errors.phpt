--TEST--
Document::$implementation createDocument errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();

try {
    $dom->implementation->createDocument("urn:a", "@");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $dom->implementation->createDocument("", "foo:bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Invalid Character Error
DOMException: Namespace Error
