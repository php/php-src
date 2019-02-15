--TEST--
Test DOMNode::insertBefore()  check the error code DOM_NOT_FOUND is raised
--DESCRIPTION--
DOMNode::insertBefore(newNode, [refNode])
DOM_NOT_FOUND is raised if refnode is not a child
This test checks the error message is raised when refnode is the parent node
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--INI--
assert.bail=true
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
$dom = new DOMDocument();

$doc = $dom->load(dirname(__FILE__) . "/book.xml", LIBXML_NOBLANKS);
assert($doc === true);

$parent_node = $dom->getElementsByTagName("book")->item(0);
assert(!is_null($parent_node));

$new_node = $dom->createElement('newnode');
assert($new_node !== false);

// getting the parent node as reference node to insert

$ref_node = $dom->getElementsByTagName("book")->item(0)->parentNode;
assert(!is_null($ref_node));

try {
    $parent_node->insertBefore($new_node, $ref_node);
} catch(DOMException $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
Not Found Error
