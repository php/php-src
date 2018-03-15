--TEST--
Bug #42082 (NodeList length zero should be empty)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument();
$xpath = new DOMXPath($doc);
$nodes = $xpath->query('*');
echo get_class($nodes), "\n";
var_dump($nodes->length);
$length = $nodes->length;
var_dump(empty($nodes->length), empty($length));

$doc->loadXML("<element></element>");
var_dump($doc->firstChild->nodeValue, empty($doc->firstChild->nodeValue), isset($doc->firstChild->nodeValue));
var_dump(empty($doc->nodeType), empty($doc->firstChild->nodeType))
?>
===DONE===
--EXPECT--
DOMNodeList
int(0)
bool(true)
bool(true)
string(0) ""
bool(true)
bool(true)
bool(false)
bool(false)
===DONE===
	
