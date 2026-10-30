--TEST--
GH-16150 (Use after free in php_dom.c)
--EXTENSIONS--
dom
--FILE--
<?php

function test($fname) {
    $e1 = new DOMElement("E1");
    $e2 = new DOMElement("E2");
    $e3 = new DOMElement("E3");
    $doc = new DOMDocument(); // Must be placed here so it is destroyed first
    $doc->{$fname}($e3);
    $e2->append($e1);
    $e3->{$fname}($e2);
    echo $doc->saveXML();
}

test('appendChild');
test('insertBefore');

?>
--EXPECT--
<?xml version="1.0"?>
<E3><E2><E1/></E2></E3>
<?xml version="1.0"?>
<E3><E2><E1/></E2></E3>
