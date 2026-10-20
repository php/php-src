--TEST--
Element::removeAttribute() edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root a="b"/>');
try {
    $dom->documentElement->removeAttributeNode($dom->createAttribute('test'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Not Found Error
