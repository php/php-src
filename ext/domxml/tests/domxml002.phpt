--TEST--
Test 2: Memleak in accessing children() of document.
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("domxml_test.inc");

$dom = xmldoc($xmlstr);
if(!$dom) {
  die('Error while parsing the document');
}

$children = $dom->children();
print_node_list($children);

?>
--EXPECT--
Node Name: 
Node Type: 14
Num Children: 1
Node Content: 

Node Name: #comment
Node Type: 8
Num Children: 0
Node Content:  lsfj  

Node Name: chapter
Node Type: 1
Num Children: 4
