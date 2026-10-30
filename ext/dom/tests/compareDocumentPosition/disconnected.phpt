--TEST--
compareDocumentPosition: disconnected
--EXTENSIONS--
dom
--FILE--
<?php

function check($element1, $element2) {
    echo "Disconnect and implementation flag (1->2): ";
    var_dump(($element1->compareDocumentPosition($element2) & (DOMNode::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | DOMNode::DOCUMENT_POSITION_DISCONNECTED)) === (DOMNode::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | DOMNode::DOCUMENT_POSITION_DISCONNECTED));
    echo "Disconnect and implementation flag (2->1): ";
    var_dump(($element2->compareDocumentPosition($element1) & (DOMNode::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | DOMNode::DOCUMENT_POSITION_DISCONNECTED)) === (DOMNode::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | DOMNode::DOCUMENT_POSITION_DISCONNECTED));
    
    // Must be the opposite result
    echo "Opposite result: ";
    if ($element1->compareDocumentPosition($element2) & DOMNode::DOCUMENT_POSITION_FOLLOWING) {
        var_dump(($element2->compareDocumentPosition($element1) & DOMNode::DOCUMENT_POSITION_PRECEDING) === DOMNode::DOCUMENT_POSITION_PRECEDING);
    } else {
        var_dump(($element2->compareDocumentPosition($element1) & DOMNode::DOCUMENT_POSITION_FOLLOWING) === DOMNode::DOCUMENT_POSITION_FOLLOWING);
    }
}

$dom1 = new DOMDocument();
$dom1->loadXML('<?xml version="1.0"?><container/>');
$dom2 = new DOMDocument();
$dom2->loadXML('<?xml version="1.0"?><container/>');

echo "--- Two documents ---\n";
check($dom1, $dom2);
echo "--- Two document roots ---\n";
check($dom1->documentElement, $dom2->documentElement);
echo "--- Fragment ---\n";
$fragment = $dom1->createDocumentFragment();
$foo = $fragment->appendChild(new DOMText("foo"));
check($foo, $dom1);
echo "--- Unattached element ---\n";
check(new DOMElement("foo"), new DOMElement("bar"));
echo "--- Unattached attribute ---\n";
check(new DOMAttr("foo"), new DOMAttr("bar"));
echo "--- Unattached attribute & element ---\n";
check(new DOMAttr("foo"), new DOMElement("bar"));

?>
--EXPECT--
--- Two documents ---
Disconnect and implementation flag (1->2): bool(true)
Disconnect and implementation flag (2->1): bool(true)
Opposite result: bool(true)
--- Two document roots ---
Disconnect and implementation flag (1->2): bool(true)
Disconnect and implementation flag (2->1): bool(true)
Opposite result: bool(true)
--- Fragment ---
Disconnect and implementation flag (1->2): bool(true)
Disconnect and implementation flag (2->1): bool(true)
Opposite result: bool(true)
--- Unattached element ---
Disconnect and implementation flag (1->2): bool(true)
Disconnect and implementation flag (2->1): bool(true)
Opposite result: bool(true)
--- Unattached attribute ---
Disconnect and implementation flag (1->2): bool(true)
Disconnect and implementation flag (2->1): bool(true)
Opposite result: bool(true)
--- Unattached attribute & element ---
Disconnect and implementation flag (1->2): bool(true)
Disconnect and implementation flag (2->1): bool(true)
Opposite result: bool(true)
