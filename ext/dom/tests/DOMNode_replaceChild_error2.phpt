--TEST--
replaceChild() where the new node is a grandparent of the old node 
--EXTENSIONS--
dom
--FILE--
<?php
$document = new DOMDocument();
$a = $document->createElement('a');
$b = $document->createElement('b');
$c = $document->createElement('c');
$a->appendChild($b);
$b->appendChild($c);
try {
	$b->replaceChild($a, $c);
} catch (DOMException $e) {
	echo "DOMException: " . $e->getMessage();
}
--EXPECT--
DOMException: Hierarchy Request Error
