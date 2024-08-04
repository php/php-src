--TEST--
Cloning a Dom\HTMLDocument
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString("<p>foo</p>");

$dom2 = clone $dom;
var_dump($dom2->firstChild->tagName);
var_dump($dom2->firstChild->textContent);

$element = $dom2->firstChild;
unset($dom2);
var_dump(get_class($element->ownerDocument));

?>
--EXPECTF--
Warning: Dom\HTMLDocument::createFromString(): tree error unexpected-token-in-initial-mode in Entity, line: 1, column: 2 in %s on line %d
string(4) "HTML"
string(3) "foo"
string(16) "Dom\HTMLDocument"
