--TEST--
Dom\HTMLDocument serialization with an imported namespace node 01
--EXTENSIONS--
dom
--FILE--
<?php

$xml = Dom\XMLDocument::createFromString('<?xml version="1.0"?><container xmlns="some:ns" xmlns:bar="another:ns"/>');
$xml->documentElement->setAttributeNS("http://foo/", "foo:bar", "value");
$xml->documentElement->appendChild($xml->createElementNS('some:ns2', 'child'));
echo $xml->saveXml(), "\n";

echo "--- After import into HTML ---\n";

$html = Dom\HTMLDocument::createFromString('<p>foo</p>', LIBXML_NOERROR);

$p = $html->documentElement->firstChild->nextSibling->firstChild;
$p->appendChild($html->importNode($xml->documentElement, true));

echo $html->saveXml(), "\n";
echo $html->saveHtml(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<container xmlns="some:ns" xmlns:bar="another:ns" xmlns:foo="http://foo/" foo:bar="value"><child xmlns="some:ns2"/></container>
--- After import into HTML ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<html xmlns="http://www.w3.org/1999/xhtml"><head></head><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns" xmlns:foo="http://foo/" foo:bar="value"><child xmlns="some:ns2"/></container></p></body></html>
<html><head></head><body><p>foo<container xmlns="some:ns" xmlns:bar="another:ns" foo:bar="value"><child></child></container></p></body></html>
