--TEST--
DOMNode: hasAttributes() 
--CREDITS--
James Lewis <james@s-1.com>
#TestFest 2008
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML($xmlstr);
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$element = $dom->documentElement;

echo "Verify that we have a DOMElement object:\n";
var_dump($element);

echo "\nElement should have attributes:\n";
var_dump($element->hasAttributes()); 

$nodelist=$dom->getElementsByTagName('tbody') ; 
$element = $nodelist->item(0);

echo "\nVerify that we have a DOMElement object:\n";
var_dump($element);

echo "\nElement should have no attributes:\n"; 
var_dump($element->hasAttributes()) 


?>
--EXPECTF--
Verify that we have a DOMElement object:
object(DOMElement)#%d (0) {
}

Element should have attributes:
bool(true)

Verify that we have a DOMElement object:
object(DOMElement)#%d (0) {
}

Element should have no attributes:
bool(false)
