--TEST--
Bug #46335 (DOMText::splitText doesn't handle multibyte characters).
--EXTENSIONS--
dom
--FILE--
<?php
$textascii = 'This is an "example" of using DOM splitText';
$text = 'This is an ‘example’ of using DOM splitText';
$start = 30;
$length = 3;

$dom = new DOMDocument('1.0', 'UTF-8');
$node = $dom->createTextNode($textascii);
$dom->appendChild($node);

print "Text: $node->textContent\n";

$matched = $node->splitText($start);
$matched->splitText($length);
print "splitText (ASCII): $matched->textContent\n";

$node = $dom->createTextNode($text);
$dom->appendChild($node);

print "Text: $node->textContent\n";

$matched = $node->splitText($start);
$matched->splitText($length);
print "splitText (UTF-8): $matched->textContent\n";
?>
--EXPECT--
Text: This is an "example" of using DOM splitText
splitText (ASCII): DOM
Text: This is an ‘example’ of using DOM splitText
splitText (UTF-8): DOM
