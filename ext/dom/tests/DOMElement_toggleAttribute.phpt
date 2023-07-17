--TEST--
DOMElement::toggleAttribute()
--EXTENSIONS--
dom
--FILE--
<?php

$html = new DOMDocument();
$html->loadHTML('<!DOCTYPE HTML><html id="test"></html>');
$xml = new DOMDocument();
$xml->loadXML('<?xml version="1.0"?><html id="test"></html>');

try {
    var_dump($html->documentElement->toggleAttribute("\0"));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- Selected attribute tests (HTML) ---\n";

var_dump($html->documentElement->toggleAttribute("SELECTED", false));
echo $html->saveHTML();
var_dump($html->documentElement->toggleAttribute("SELECTED"));
echo $html->saveHTML();
var_dump($html->documentElement->toggleAttribute("selected", true));
echo $html->saveHTML();
var_dump($html->documentElement->toggleAttribute("selected"));
echo $html->saveHTML();

echo "--- Selected attribute tests (XML) ---\n";

var_dump($xml->documentElement->toggleAttribute("SELECTED", false));
echo $xml->saveXML();
var_dump($xml->documentElement->toggleAttribute("SELECTED"));
echo $xml->saveXML();
var_dump($xml->documentElement->toggleAttribute("selected", true));
echo $xml->saveXML();
var_dump($xml->documentElement->toggleAttribute("selected"));
echo $xml->saveXML();

echo "--- id attribute tests ---\n";

var_dump($html->getElementById("test") === NULL);
var_dump($html->documentElement->toggleAttribute("id"));
var_dump($html->getElementById("test") === NULL);

echo "--- Namespace tests ---\n";

$dom = new DOMDocument();
$dom->loadXML("<?xml version='1.0'?><container xmlns='some:ns' xmlns:foo='some:ns2' xmlns:anotherone='some:ns3'><foo:bar/><baz/></container>");

echo "Toggling namespaces:\n";
var_dump($dom->documentElement->toggleAttribute('xmlns'));
echo $dom->saveXML();
var_dump($dom->documentElement->toggleAttribute('xmlns:anotherone'));
echo $dom->saveXML();
var_dump($dom->documentElement->toggleAttribute('xmlns:anotherone'));
echo $dom->saveXML();
var_dump($dom->documentElement->toggleAttribute('xmlns:foo'));
echo $dom->saveXML();
var_dump($dom->documentElement->toggleAttribute('xmlns:nope', false));
echo $dom->saveXML();

echo "Toggling namespaced attributes:\n";
var_dump($dom->documentElement->toggleAttribute('test:test'));
var_dump($dom->documentElement->firstElementChild->toggleAttribute('foo:test'));
var_dump($dom->documentElement->firstElementChild->toggleAttribute('doesnotexist:test'));
var_dump($dom->documentElement->firstElementChild->toggleAttribute('doesnotexist:test2', false));
echo $dom->saveXML();

echo "namespace of test:test = ";
var_dump($dom->documentElement->getAttributeNode('test:test')->namespaceURI);
echo "namespace of foo:test = ";
var_dump($dom->documentElement->firstElementChild->getAttributeNode('foo:test')->namespaceURI);
echo "namespace of doesnotexist:test = ";
var_dump($dom->documentElement->firstElementChild->getAttributeNode('doesnotexist:test')->namespaceURI);

echo "Toggling namespaced attributes:\n";
var_dump($dom->documentElement->toggleAttribute('test:test'));
var_dump($dom->documentElement->firstElementChild->toggleAttribute('foo:test'));
var_dump($dom->documentElement->firstElementChild->toggleAttribute('doesnotexist:test'));
var_dump($dom->documentElement->firstElementChild->toggleAttribute('doesnotexist:test2', true));
var_dump($dom->documentElement->firstElementChild->toggleAttribute('doesnotexist:test3', false));
echo $dom->saveXML();

echo "Checking toggled namespace:\n";
var_dump($dom->documentElement->getAttribute('xmlns:anotheron'));

?>
--EXPECT--
Invalid Character Error
--- Selected attribute tests (HTML) ---
bool(false)
<!DOCTYPE HTML>
<html id="test"></html>
bool(true)
<!DOCTYPE HTML>
<html id="test" selected></html>
bool(true)
<!DOCTYPE HTML>
<html id="test" selected></html>
bool(false)
<!DOCTYPE HTML>
<html id="test"></html>
--- Selected attribute tests (XML) ---
bool(false)
<?xml version="1.0"?>
<html id="test"/>
bool(true)
<?xml version="1.0"?>
<html id="test" SELECTED=""/>
bool(true)
<?xml version="1.0"?>
<html id="test" SELECTED="" selected=""/>
bool(false)
<?xml version="1.0"?>
<html id="test" SELECTED=""/>
--- id attribute tests ---
bool(false)
bool(false)
bool(true)
--- Namespace tests ---
Toggling namespaces:
bool(false)
<?xml version="1.0"?>
<container xmlns:foo="some:ns2" xmlns:anotherone="some:ns3" xmlns="some:ns"><foo:bar/><baz/></container>
bool(false)
<?xml version="1.0"?>
<container xmlns:foo="some:ns2" xmlns="some:ns"><foo:bar/><baz/></container>
bool(true)
<?xml version="1.0"?>
<container xmlns:foo="some:ns2" xmlns="some:ns" xmlns:anotherone=""><foo:bar/><baz/></container>
bool(false)
<?xml version="1.0"?>
<container xmlns="some:ns" xmlns:anotherone=""><foo:bar xmlns:foo="some:ns2"/><baz/></container>
bool(false)
<?xml version="1.0"?>
<container xmlns="some:ns" xmlns:anotherone=""><foo:bar xmlns:foo="some:ns2"/><baz/></container>
Toggling namespaced attributes:
bool(true)
bool(true)
bool(true)
bool(false)
<?xml version="1.0"?>
<container xmlns="some:ns" xmlns:anotherone="" test:test=""><foo:bar xmlns:foo="some:ns2" foo:test="" doesnotexist:test=""/><baz/></container>
namespace of test:test = NULL
namespace of foo:test = string(8) "some:ns2"
namespace of doesnotexist:test = NULL
Toggling namespaced attributes:
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
<?xml version="1.0"?>
<container xmlns="some:ns" xmlns:anotherone=""><foo:bar xmlns:foo="some:ns2" doesnotexist:test2=""/><baz/></container>
Checking toggled namespace:
string(0) ""
