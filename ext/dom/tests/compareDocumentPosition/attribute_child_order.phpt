--TEST--
compareDocumentPosition: attribute vs child order
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container>
<before/>
<outer align="center">
    <p>foo</p>
    <div>
        <div>
            <p>bar</p>
        </div>
    </div>
</outer>
</container>
XML);

$before = $dom->documentElement->firstElementChild;
$outer = $before->nextElementSibling;
$foo = $outer->firstElementChild;
$bar = $foo->nextElementSibling->firstElementChild->firstElementChild;

// See note about attributes vs children positions: attributes precede children

echo "--- outer attribute vs before ---\n";
var_dump($outer->attributes[0]->compareDocumentPosition($before) === DOMNode::DOCUMENT_POSITION_PRECEDING);
var_dump($before->compareDocumentPosition($outer->attributes[0]) === DOMNode::DOCUMENT_POSITION_FOLLOWING);

echo "--- outer attribute vs foo ---\n";
var_dump($outer->attributes[0]->compareDocumentPosition($foo) === DOMNode::DOCUMENT_POSITION_FOLLOWING);
var_dump($foo->compareDocumentPosition($outer->attributes[0]) === DOMNode::DOCUMENT_POSITION_PRECEDING);

echo "--- outer attribute vs bar ---\n";
var_dump($outer->attributes[0]->compareDocumentPosition($bar) === DOMNode::DOCUMENT_POSITION_FOLLOWING);
var_dump($bar->compareDocumentPosition($outer->attributes[0]) === DOMNode::DOCUMENT_POSITION_PRECEDING);

?>
--EXPECT--
--- outer attribute vs before ---
bool(true)
bool(true)
--- outer attribute vs foo ---
bool(true)
bool(true)
--- outer attribute vs bar ---
bool(true)
bool(true)
