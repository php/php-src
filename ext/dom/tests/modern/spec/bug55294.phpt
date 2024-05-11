--TEST--
Bug #55294 (DOMDocument::importNode shifts namespaces when "default" namespace exists)
--EXTENSIONS--
dom
--FILE--
<?php

$aDOM = Dom\XMLDocument::createFromString(<<<EOXML
<A xmlns="http://example.com/A">
<B>
<C xmlns="http://example.com/C" xmlns:default="http://example.com/Z" />
</B>
</A>
EOXML
);

$bDOM = Dom\XMLDocument::createEmpty();
$node = $bDOM->importNode($aDOM->getElementsByTagNameNS('http://example.com/A', 'B')->item(0), true);
$bDOM->appendChild($node);

echo $bDOM->saveXml(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<B xmlns="http://example.com/A">
<C xmlns="http://example.com/C" xmlns:default="http://example.com/Z"/>
</B>
