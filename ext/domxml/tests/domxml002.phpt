--TEST--
Test 2: Memleak in accessing children() of document.
--INI--
report_memleaks=0
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
--------- root
Node Name: chapter
Node Type: 1
Num Children: 4