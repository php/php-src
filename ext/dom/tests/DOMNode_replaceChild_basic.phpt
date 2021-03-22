--TEST--
Replacing a child node
--EXTENSIONS--
dom
--CREDITS--
Matt Raines <matt@raines.me.uk>
#London TestFest 2008
--FILE--
<?php
$document = new DOMDocument();
$document->loadXML('<?xml version="1.0" encoding="utf-8"?>
<root><foo><bar/><baz/></foo><spam><eggs/><eggs/></spam></root>');

// Replaces the child node oldChild with newChild in the list of children, and
// returns the oldChild node.
$parent = $document->getElementsByTagName('foo')->item(0);
$new_child = $document->createElement('qux');
$old_child = $parent->replaceChild($new_child, $parent->firstChild);
echo "New child replaces old child:\n" . $document->saveXML();
echo "Old child is returned:\n" . $old_child->tagName . "\n";

// If the newChild is already in the tree, it is first removed.
$parent = $document->getElementsByTagName('spam')->item(0);
$parent->replaceChild($new_child, $parent->firstChild);
echo "Existing child is removed from tree:\n" . $document->saveXML();

// Children are inserted in the correct order.
$new_child = $document->getElementsByTagName('spam')->item(0);
$parent = $document->getElementsByTagName('foo')->item(0);
$parent->replaceChild($new_child, $parent->firstChild);
echo "Children are inserted in order:\n" . $document->saveXML();
?>
--EXPECT--
New child replaces old child:
<?xml version="1.0" encoding="utf-8"?>
<root><foo><qux/><baz/></foo><spam><eggs/><eggs/></spam></root>
Old child is returned:
bar
Existing child is removed from tree:
<?xml version="1.0" encoding="utf-8"?>
<root><foo><baz/></foo><spam><qux/><eggs/></spam></root>
Children are inserted in order:
<?xml version="1.0" encoding="utf-8"?>
<root><foo><spam><qux/><eggs/></spam></foo></root>
