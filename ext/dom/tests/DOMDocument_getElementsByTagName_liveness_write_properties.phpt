--TEST--
DOMDocument::getElementsByTagName() liveness affected by writing properties
--EXTENSIONS--
dom
--FILE--
<?php

$xml = '<root xmlns:ns1="foo" xmlns:ns2="bar"><ns1:a/></root>';
$fields = ['nodeValue', 'textContent'];

foreach ($fields as $field) {
    $doc = new DOMDocument;
    $doc->loadXML($xml);
    $list = $doc->getElementsByTagName('a');
    var_dump($list->item(0) === NULL);
    $doc->documentElement->{$field} = 'new_content';
    var_dump($list->item(0) === NULL);
    print $doc->saveXML();
}

// Shouldn't be affected
$doc = new DOMDocument;
$doc->loadXML($xml);
$list = $doc->getElementsByTagNameNS('foo', 'a');
var_dump($list->item(0) === NULL);
$doc->documentElement->firstChild->prefix = 'ns2';
var_dump($list->item(0) === NULL);
print $doc->saveXML();

?>
--EXPECT--
bool(false)
bool(true)
<?xml version="1.0"?>
<root xmlns:ns1="foo" xmlns:ns2="bar">new_content</root>
bool(false)
bool(true)
<?xml version="1.0"?>
<root xmlns:ns1="foo" xmlns:ns2="bar">new_content</root>
bool(false)
bool(false)
<?xml version="1.0"?>
<root xmlns:ns1="foo" xmlns:ns2="bar"><ns2:a xmlns:ns2="foo"/></root>
