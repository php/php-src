--TEST--
Bug #25900 (DomDocument->get_elements_by_tagname() doesn't work with default ns)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$xml = <<<HERE
<?xml version="1.0" encoding="ISO-8859-1" ?>
<foo xmlns="http://www.example.com/ns/foo">
  <bar><test1 /></bar>
  <bar><test2 /></bar>
</foo>
HERE;

function dump($elems) {
	foreach ($elems as $elem) {
		var_dump($elem->node_name());
		if ($elem->node_type() == XML_ELEMENT_NODE) {
			dump($elem->child_nodes());
		}
	}
}

$dom = xmldoc($xml);
$doc = $dom->document_element();
dump($dom->get_elements_by_tagname('bar'));
dump($doc->get_elements_by_tagname('bar'));
?>
--EXPECT--
string(3) "bar"
string(5) "test1"
string(3) "bar"
string(5) "test2"
string(3) "bar"
string(5) "test1"
string(3) "bar"
string(5) "test2"
