--TEST--
Handling fragments of multiple nodes for DOMParentNode
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML('<!DOCTYPE HTML><html><container/></html>');

$container = $dom->documentElement->firstElementChild;

$fragment = $dom->createDocumentFragment();
$fragment->appendChild($dom->createElement('p', '1'));
$fragment->appendChild($dom->createElement('b', '2'));
$container->replaceWith($fragment);
echo $dom->saveXML();

$dom->documentElement->append('foo');
echo $dom->saveXML();

$fragment = $dom->createDocumentFragment();
$fragment->appendChild($dom->createElement('p', '3'));
$fragment->appendChild($dom->createElement('b', '4'));
$dom->documentElement->prepend($fragment);
echo $dom->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<!DOCTYPE HTML>
<html><p>1</p><b>2</b></html>
<?xml version="1.0"?>
<!DOCTYPE HTML>
<html><p>1</p><b>2</b>foo</html>
<?xml version="1.0"?>
<!DOCTYPE HTML>
<html><p>3</p><b>4</b><p>1</p><b>2</b>foo</html>
