--TEST--
DOMNode::getNodePath()
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
    var_dump($node->getNodePath());
}
?>
--EXPECT--
string(20) "/books/book[1]/title"
string(20) "/books/book[2]/title"
