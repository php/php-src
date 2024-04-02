--TEST--
DOM\XMLDocument::saveXML(File) node
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty("1.0", "ASCII");
$root = $dom->appendChild($dom->createElement("root"));
$child1 = $root->appendChild($dom->createElement("child1"));
$child2 = $root->appendChild($dom->createElement("child2"));
echo $dom->saveXML($child1);

?>
--EXPECT--
<child1/>
