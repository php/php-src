--TEST--
GH-23118 (Stack overflow when freeing a deeply nested detached subtree)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = Dom\XMLDocument::createEmpty();
$node = $doc->createElement('a');
for ($i = 0; $i < 100000; $i++) {
    $parent = $doc->createElement('a');
    $parent->appendChild($node);
    $node = $parent;
}
echo "built", PHP_EOL;
unset($parent, $node);
echo "freed", PHP_EOL;
?>
--EXPECT--
built
freed
