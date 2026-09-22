--TEST--
GH-18744 (classList works not correctly if copy HTMLElement by clone keyword.)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = \Dom\HTMLDocument::createEmpty();
$ele1 = $doc->createElement('div');
$ele1->classList->add('foo');
$ele2 = clone $ele1;
$ele2->classList->add('bar');

echo "Element1 class: " . $ele1->getAttribute('class');
echo "\n";
echo "Element2 class: " . $ele2->getAttribute('class');
echo "\n";

var_dump($ele1->classList !== $ele2->classList);
// These comparisons are not pointless: they're getters and should not create new objects
var_dump($ele1->classList === $ele1->classList);
var_dump($ele2->classList === $ele2->classList);

?>
--EXPECT--
Element1 class: foo
Element2 class: foo bar
bool(true)
bool(true)
bool(true)
