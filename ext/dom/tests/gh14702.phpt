--TEST--
GH-14702 (DOMDocument::xinclude() crash)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<root>
  <child/>
  <include href="foo" xmlns="http://www.w3.org/2001/XInclude">
    <fallback/>
  </include>
  <keep/>
</root>
XML);
$xi = $doc->createElementNS('http://www.w3.org/2001/XInclude', 'xi:include');
$xi->setAttribute('href', 'nonexistent');

$fallback = $doc->createElementNS('http://www.w3.org/2001/XInclude', 'xi:fallback');
$xi->appendChild($fallback);
$child1 = $fallback->appendChild($doc->createElement('fallback-child1'));
$child2 = $fallback->appendChild($doc->createElement('fallback-child2'));

$xpath = new DOMXPath($doc);
$toReplace = $xpath->query('//child')->item(0);
$toReplace->parentNode->replaceChild($xi, $toReplace);

$keep = $doc->documentElement->lastElementChild;

var_dump(@$doc->xinclude());
echo $doc->saveXML();

var_dump($child1, $child2, $fallback, $keep->nodeName);

$keep->textContent = 'still works';
echo $doc->saveXML();
?>
--EXPECT--
int(2)
<?xml version="1.0"?>
<root>
  <fallback-child1/><fallback-child2/>
  
  <keep/>
</root>
object(DOMElement)#4 (1) {
  ["schemaTypeInfo"]=>
  NULL
}
object(DOMElement)#5 (1) {
  ["schemaTypeInfo"]=>
  NULL
}
object(DOMElement)#3 (1) {
  ["schemaTypeInfo"]=>
  NULL
}
string(4) "keep"
<?xml version="1.0"?>
<root>
  <fallback-child1/><fallback-child2/>
  
  <keep>still works</keep>
</root>
