--TEST--
DOMNode: isSameNode() 
--CREDITS--
James Lewis <james@s-1.com>
#TestFest 2008
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML($xmlstr);
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$node = $dom->documentElement;
if($node->isSameNode($node)) 
	echo "EXPECTING SAME NODE, PASSED\n" ; 
else
	echo "EXPECTING SAME NODE, FAILED\n" ; 

$nodelist=$dom->getElementsByTagName('tbody') ; 

if($nodelist->item(0)->isSameNode($node))
	echo "EXPECTING NOT SAME NODE, FAILED\n" ; 
else
	echo "EXPECTING NOT SAME NODE, PASSED\n" ; 
	
?>
===DONE===
--EXPECT--
EXPECTING SAME NODE, PASSED
EXPECTING NOT SAME NODE, PASSED
===DONE===
