--TEST--
DOMNamedNodeMap iterator key() after the last item
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<!DOCTYPE root [<!ENTITY e "x"><!NOTATION n SYSTEM "n">]><root a="1"/>');
$it = $doc->documentElement->attributes->getIterator();
echo "DOMNamedNodeMap attributes first key: ";
var_dump($it->key());
$it->next();
echo "DOMNamedNodeMap attributes key after end: ";
var_dump($it->key());

$it = $doc->doctype->entities->getIterator();
echo "DOMNamedNodeMap entities first key: ";
var_dump($it->key());
$it->next();
echo "DOMNamedNodeMap entities key after end: ";
var_dump($it->key());

$it = $doc->doctype->notations->getIterator();
echo "DOMNamedNodeMap notations first key: ";
var_dump($it->key());
$it->next();
echo "DOMNamedNodeMap notations key after end: ";
var_dump($it->key());

$doc = Dom\XMLDocument::createFromString('<!DOCTYPE root [<!ENTITY e "x"><!NOTATION n SYSTEM "n">]><root a="1"/>');
$it = $doc->documentElement->attributes->getIterator();
echo "Dom\NamedNodeMap first key: ";
var_dump($it->key());
$it->next();
echo "Dom\NamedNodeMap key after end: ";
var_dump($it->key());

$it = $doc->doctype->entities->getIterator();
echo "Dom\DtdNamedNodeMap entities first key: ";
var_dump($it->key());
$it->next();
echo "Dom\DtdNamedNodeMap entities key after end: ";
var_dump($it->key());

$it = $doc->doctype->notations->getIterator();
echo "Dom\DtdNamedNodeMap notations first key: ";
var_dump($it->key());
$it->next();
echo "Dom\DtdNamedNodeMap notations key after end: ";
var_dump($it->key());

?>
--EXPECT--
DOMNamedNodeMap attributes first key: string(1) "a"
DOMNamedNodeMap attributes key after end: NULL
DOMNamedNodeMap entities first key: string(1) "e"
DOMNamedNodeMap entities key after end: NULL
DOMNamedNodeMap notations first key: string(1) "n"
DOMNamedNodeMap notations key after end: NULL
Dom\NamedNodeMap first key: string(1) "a"
Dom\NamedNodeMap key after end: NULL
Dom\DtdNamedNodeMap entities first key: string(1) "e"
Dom\DtdNamedNodeMap entities key after end: NULL
Dom\DtdNamedNodeMap notations first key: string(1) "n"
Dom\DtdNamedNodeMap notations key after end: NULL
