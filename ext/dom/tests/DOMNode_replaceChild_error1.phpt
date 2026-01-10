--TEST--
replaceChild() where the old node is not a child
--EXTENSIONS--
dom
--FILE--
<?php
$document = new DOMDocument();
$real_parent = $document->createElement('real');
$parent = $document->createElement('p');
$child1 = $document->createElement('child1');
$child2 = $document->createElement('child2');
$child3 = $document->createElement('child3');
$real_parent->appendChild($child1);
$parent->appendChild($child2);
try {
	$parent->replaceChild($child3, $child1);
} catch (DOMException $e) {
	echo "DOMException: " . $e->getMessage();
}
?>
--EXPECT--
DOMException: Not Found Error
