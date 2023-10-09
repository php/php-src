--TEST--
DOMDocument node list length cache invalidation
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadHTML('<p>hello</p><p>world</p><p>!</p>');

$elements = $doc->getElementsByTagName('p');
$item = $elements->item(0); // Activate item cache
var_dump($elements->length); // Length not cached yet, should still compute
$item->remove();
// Now element 0 means "world", and 1 means "!"
unset($item);
var_dump($elements->length);
$item = $elements->item(1);
var_dump($item->textContent);
$item = $elements->item(1);
var_dump($item->textContent);
$item = $elements->item(0);
var_dump($item->textContent);
$item = $elements->item(1);
var_dump($item->textContent);

?>
--EXPECT--
int(3)
int(2)
string(1) "!"
string(1) "!"
string(5) "world"
string(1) "!"
