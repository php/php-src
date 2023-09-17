--TEST--
DOM\HTMLDocument serialization of element in a namespace
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElement("root"));

$root->append("\n");
$root->append($dom->createElementNS("http://php.net", "noprefix"), "\n");
$root->append($dom->createElementNS("http://php.net", "with:prefix"), "\n");
$root->append($dom->createElementNS("http://www.w3.org/1999/xhtml", "xhtml:br"), "\n");
$root->append($dom->createElementNS("http://www.w3.org/2000/svg", "s:svg"), "\n");
$root->append($dom->createElementNS("http://www.w3.org/1998/Math/MathML", "m:math"), "\n");

// Note: XML declarations are not emitted in HTML5
echo $dom->saveHTML();

?>
--EXPECT--
<root>
<noprefix></noprefix>
<with:prefix></with:prefix>
<br>
<svg></svg>
<math></math>
</root>
