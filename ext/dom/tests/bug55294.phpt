--TEST--
Bug #55294 (DOMDocument::importNode shifts namespaces when "default" namespace exists)
--EXTENSIONS--
dom
--XFAIL--
See https://github.com/php/php-src/pull/12308
--FILE--
<?php

$aDOM = new DOMDocument();
$aDOM->loadXML(<<<EOXML
<A xmlns="http://example.com/A">
<B>
<C xmlns="http://example.com/C" xmlns:default="http://example.com/Z" />
</B>
</A>
EOXML
);

$bDOM = new DOMDocument();
$node = $bDOM->importNode($aDOM->getElementsByTagNameNS('http://example.com/A', 'B')->item(0), true);
$bDOM->appendChild($node);

echo $bDOM->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0"?>
<B xmlns="http://example.com/A">
<C xmlns="http://example.com/C" xmlns:default="http://example.com/Z"/>
</B>
