--TEST--
Element::removeAttribute() edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root a="b"/>');
try {
    $dom->documentElement->removeAttributeNode($dom->createAttribute('test'));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Not Found Error
