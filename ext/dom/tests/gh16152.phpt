--TEST--
GH-16152 (Memory leak in DOMProcessingInstruction/DOMDocument)
--EXTENSIONS--
dom
--FILE--
<?php

function test($fname) {
    $doc = new DOMDocument();
    $instr = new DOMProcessingInstruction("tr", "r");
    $frag = new DOMDocumentFragment();
    $frag2 = new DOMDocumentFragment();
    $frag2->append($instr);
    $frag->append($frag2);
    $doc->{$fname}($frag);
    echo $doc->saveXML();
}

test('insertBefore');
test('appendChild');

?>
--EXPECT--
<?xml version="1.0"?>
<?tr r?>
<?xml version="1.0"?>
<?tr r?>
