--TEST--
GH-11404 (DOMDocument::savexml and friends ommit xmlns="" declaration for null namespace, creating incorrect xml representation of the DOM)
--EXTENSIONS--
dom
--FILE--
<?php

$dom1 = Dom\XMLDocument::createFromString('<?xml version="1.0" ?><with xmlns="some:ns" />');

$dom2 = Dom\XMLDocument::createFromString('<?xml version="1.0" ?><none />');

$dom1->documentElement->append(
    $dom1->importNode(
        $dom2->documentElement
    )
);

echo $dom1->saveXml();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<with xmlns="some:ns"><none xmlns=""/></with>
