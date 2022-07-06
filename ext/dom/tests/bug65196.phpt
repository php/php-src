--TEST--
bug #65196 (Passing DOMDocumentFragment to DOMDocument::saveHTML() Produces invalid Markup)
--SKIPIF--
<?php
extension_loaded("dom") or die("skip need ext/dom");
?>
--FILE--
<?php
$dom = new DOMDocument();

$frag1 = $dom->createDocumentFragment();
var_dump($dom->saveHTML($frag1));

$frag2 = $dom->createDocumentFragment();
$div = $dom->createElement('div');
$div->appendChild($dom->createElement('span'));
$frag2->appendChild($div);
$frag2->appendChild($dom->createElement('div'));
$frag2->appendChild($dom->createElement('div'));
var_dump($dom->saveHTML($frag2));
?>
--EXPECT--
string(0) ""
string(46) "<div><span></span></div><div></div><div></div>"
