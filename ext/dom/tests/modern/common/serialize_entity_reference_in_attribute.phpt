--TEST--
Serialize entity reference within attribute
--EXTENSIONS--
dom
--FILE--
<?php

$xml = Dom\XMLDocument::createFromString(<<<XML
<!DOCTYPE root [
    <!ENTITY foo "foo">
]>
<root><el x="&foo;bar&foo;"/></root>
XML);

$el = $xml->documentElement->firstChild;
echo $xml->saveXml(), "\n";

$html = Dom\HTMLDocument::createEmpty();
$html->append($html->importNode($el, true));
echo $html->saveHtml(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
<!ENTITY foo "foo">
]>
<root><el x="&foo;bar&foo;"/></root>
<el x="&foo;bar&foo;"></el>
