--TEST--
Document::$implementation createDocument errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();

try {
    $dom->implementation->createDocument("urn:a", "@");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $dom->implementation->createDocument("", "foo:bar");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Invalid Character Error
Namespace Error
