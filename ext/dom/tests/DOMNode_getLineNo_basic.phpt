--TEST--
DOMNode::getLineNo()
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php
$file = __DIR__.'/book.xml';
$doc = new DOMDocument();
$doc->load($file);
$nodes = $doc->getElementsByTagName('title');
foreach($nodes as $node) {
	var_dump($node->getLineNo());
}
?>
--EXPECTF--
int(4)
int(8)
