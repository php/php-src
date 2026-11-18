--TEST--
Dom\import_simplexml() refuses a node that already carries a legacy object
--EXTENSIONS--
dom
simplexml
--FILE--
<?php

// Reached through a genuine SimpleXMLElement: the node already carries a
// DOMElement, so returning it would violate the Attr|Element return type.
$doc = new DOMDocument;
$doc->loadXML('<r z="3"><a/></r>');
$element = $doc->documentElement;
$sxe = simplexml_import_dom($element);
try {
    Dom\import_simplexml($sxe);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

// Same for an attribute node, which SimpleXML refuses to import, so it is
// passed directly.
$attr = $doc->documentElement->getAttributeNode('z');
try {
    Dom\import_simplexml($attr);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

// The document keeps its model: it must not have been relabelled on the way out.
var_dump(get_class($doc->createElement('z')));

// A node with no legacy object attached is still imported normally.
$clean = simplexml_load_string('<r/>');
var_dump(get_class(Dom\import_simplexml($clean)));

?>
--EXPECT--
Dom\import_simplexml(): Argument #1 ($node) must not be already imported as a DOMNode
Dom\import_simplexml(): Argument #1 ($node) must not be already imported as a DOMNode
string(10) "DOMElement"
string(11) "Dom\Element"
