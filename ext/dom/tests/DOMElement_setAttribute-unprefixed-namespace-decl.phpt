--TEST--
DOMNode: setAttribute() with unprefixed namespace declaration
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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
echo get_class($element), "\n";

echo "\nElement should not have xmlns attribute:\n";
var_dump($element->hasAttribute('xmlns'));

echo "\nElement should not have namespace:\n";
var_dump($element->lookupPrefix('http://www.php.net/'), $element->lookupNamespaceURI(null));

echo "\nsetAttribute() call should succeed:\n";
var_dump($element->setAttribute('xmlns', 'http://www.php.net/'));

echo "\nElement should have xmlns attribute:\n";
var_dump($element->hasAttribute('xmlns'));

echo "\nElement should have namespace:\n";
var_dump($element->lookupPrefix('http://www.php.net/'), $element->lookupNamespaceURI(null));

?>
--EXPECTF--
Verify that we have a DOMElement object:
DOMElement

Element should not have xmlns attribute:
bool(false)

Element should not have namespace:
NULL
NULL

setAttribute() call should succeed:
bool(true)

Element should have xmlns attribute:
bool(true)

Element should have namespace:
NULL
string(19) "http://www.php.net/"
