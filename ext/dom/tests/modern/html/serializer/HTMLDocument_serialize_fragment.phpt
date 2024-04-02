--TEST--
DOM\HTMLDocument serialization of document fragment
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$fragment = $dom->createDocumentFragment();
$fragment->appendChild($dom->createElement("foo"));
$bar = $fragment->appendChild($dom->createElement("bar"));
$fragment->appendChild($dom->createElement("baz"));
$bar->appendChild($dom->createElement("inner"));
echo $dom->saveHTML($fragment);

?>
--EXPECT--
<foo></foo><bar><inner></inner></bar><baz></baz>
