--TEST--
Bug #69373 References to deleted XPath query results
--SKIPIF--
<?php require 'skipif.inc' ?>
--FILE--
<?php
$doc = new DOMDocument();
for( $i=0; $i<20; $i++ ) {
	$doc->loadXML("<parent><child /><child /></parent>");
	$xpath = new DOMXpath($doc);
	$all = $xpath->query('//*');
	$doc->firstChild->nodeValue = '';
}
echo 'DONE', PHP_EOL;
?>
--EXPECT--
DONE
