--TEST--
GH-21486 (Dom\HTMLDocument parser mangles xml:space and xml:lang attributes)
--EXTENSIONS--
dom
--CREDITS--
JKingweb
--FILE--
<?php
$d = @\Dom\HTMLDocument::createFromString("<div></div>");
$e = $d->getElementsByTagName("div")[0];
$e->innerHTML = '<svg xml:space="default" xlink:href="about:blank" xmlns:foo="barspace"></svg>';
$svg = $d->querySelector("svg");
echo $e->innerHTML."\n";
echo $svg->attributes[0]->localName." ".var_export($svg->attributes[0]->namespaceURI, true)."\n";
?>
--EXPECT--
<svg xml:space="default" xlink:href="about:blank" xmlns:foo="barspace"></svg>
space 'http://www.w3.org/XML/1998/namespace'
