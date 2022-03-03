--TEST--
Test DOMNode::insertBefore()  check the error code DOM_NOT_FOUND is raised
--DESCRIPTION--
DOMNode::insertBefore(newNode, [refNode])
DOM_NOT_FOUND is raised if refnode is not a child
This test checks the error message is raised when the refnode is a descendant but not a child
--CREDITS--
Antonio Diaz Ruiz <dejalatele@gmail.com>
--INI--
assert.bail=true
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument();

$doc = $dom->load(__DIR__ . "/book.xml", LIBXML_NOBLANKS);
assert($doc === true);

$parent_node = $dom->getElementsByTagName("book")->item(0);
assert(!is_null($parent_node));

$new_node = $dom->createElement('newnode');
assert($new_node !== false);

// creating a new node (descendant) and getting it as the refnode

$ref_node = $dom->createElement('newnode3');
$parent_node->childNodes->item(0)->appendChild($ref_node);
$dom->saveXML();

try {
    $parent_node->insertBefore($new_node, $ref_node);
} catch(DOMException $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Not Found Error
