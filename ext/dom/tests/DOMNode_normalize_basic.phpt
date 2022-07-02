--TEST--
DOMNode::normalize()
--EXTENSIONS--
dom
--FILE--
<?php

/* Create an XML document
 * with structure
 * <book>
 *  <author></author>
 *  <title>This is the title</title>
 * </book>
 * Calculate the number of title text nodes (1).
 * Add another text node to title. Calculate the number of title text nodes (2).
 * Normalize author. Calculate the number of title text nodes (2).
 * Normalize title. Calculate the number of title text nodes (1).
*/

$doc = new DOMDocument();

$root = $doc->createElement('book');
$doc->appendChild($root);

$title = $doc->createElement('title');
$root->appendChild($title);

$author = $doc->createElement('author');
$root->appendChild($author);

$text = $doc->createTextNode('This is the first title');
$title->appendChild($text);

echo "Number of child nodes of title = ";
var_dump($title->childNodes->length);

// add a second text node to title
$text = $doc->createTextNode('This is the second title');
$title->appendChild($text);

echo "Number of child nodes of title after adding second title = ";
var_dump($title->childNodes->length);

// should do nothing
$author->normalize();

echo "Number of child nodes of title after normalizing author = ";
var_dump($title->childNodes->length);


// should concatenate first and second title text nodes
$title->normalize();

echo "Number of child nodes of title after normalizing title = ";
var_dump($title->childNodes->length);

?>
--EXPECT--
Number of child nodes of title = int(1)
Number of child nodes of title after adding second title = int(2)
Number of child nodes of title after normalizing author = int(2)
Number of child nodes of title after normalizing title = int(1)
