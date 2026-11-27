--TEST--
DOMNamedNodeMap iterator key() on an empty DTD named node map
--EXTENSIONS--
dom
--FILE--
<?php

$xml = '<!DOCTYPE root><root/>';

$doc = new DOMDocument();
$doc->loadXML($xml);
echo "DOMNamedNodeMap entities valid: ";
var_dump($doc->doctype->entities->getIterator()->valid());
echo "DOMNamedNodeMap entities key: ";
var_dump($doc->doctype->entities->getIterator()->key());
echo "DOMNamedNodeMap notations key: ";
var_dump($doc->doctype->notations->getIterator()->key());

$doc = Dom\XMLDocument::createFromString($xml);
echo "Dom\DtdNamedNodeMap entities valid: ";
var_dump($doc->doctype->entities->getIterator()->valid());
echo "Dom\DtdNamedNodeMap entities key: ";
var_dump($doc->doctype->entities->getIterator()->key());
echo "Dom\DtdNamedNodeMap notations key: ";
var_dump($doc->doctype->notations->getIterator()->key());

?>
--EXPECT--
DOMNamedNodeMap entities valid: bool(false)
DOMNamedNodeMap entities key: NULL
DOMNamedNodeMap notations key: NULL
Dom\DtdNamedNodeMap entities valid: bool(false)
Dom\DtdNamedNodeMap entities key: NULL
Dom\DtdNamedNodeMap notations key: NULL
