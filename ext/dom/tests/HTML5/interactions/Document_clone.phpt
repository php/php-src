--TEST--
Cloning a DOM\HTML5Document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->loadHTML("<p>foo</p>");

$dom2 = clone $dom;
var_dump($dom2->firstChild->tagName);
var_dump($dom2->firstChild->textContent);
$dom2->loadHTML("<strong>bar</strong>");
var_dump($dom2->firstChild->tagName);
var_dump($dom2->firstChild->textContent);

$element = $dom2->firstChild;
unset($dom2);
var_dump(get_class($element->ownerDocument));

?>
--EXPECTF--
Warning: DOM\HTML5Document::loadHTML(): tree error unexpected-token-in-initial-mode in Entity, line: 1, column: 2 in %s on line %d
string(4) "html"
string(3) "foo"

Warning: DOM\HTML5Document::loadHTML(): tree error unexpected-token-in-initial-mode in Entity, line: 1, column: 2-7 in %s on line %d
string(4) "html"
string(3) "bar"
string(17) "DOM\HTML5Document"
