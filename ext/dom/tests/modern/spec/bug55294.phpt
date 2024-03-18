--TEST--
Bug #55294 (DOMDocument::importNode shifts namespaces when "default" namespace exists)
--EXTENSIONS--
dom
--FILE--
<?php

$aDOM = DOM\XMLDocument::createFromString(<<<EOXML
<A xmlns="http://example.com/A">
<B>
<C xmlns="http://example.com/C" xmlns:default="http://example.com/Z" />
</B>
</A>
EOXML
);

$bDOM = DOM\XMLDocument::createEmpty();
$node = $bDOM->importNode($aDOM->getElementsByTagNameNS('http://example.com/A', 'B')->item(0), true);
$bDOM->appendChild($node);

echo $bDOM->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<B xmlns="http://example.com/A">
<C xmlns="http://example.com/C" xmlns:default="http://example.com/Z"/>
</B>
