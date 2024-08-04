--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - prepending variation
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument();
$root = $dom->createElement('html');
$dom->appendChild($root);

$el1 = $dom->createElement('p1');
$el1->setAttribute('id', 'foo');
$el1->setIdAttribute('id', true);

$root->appendChild($el1);

$el2 = $dom->createElement('p2');
$el2->setAttribute('id', 'foo');
$el2->setIdAttribute('id', true);

$root->appendChild($el2);
unset($el1, $el2);

$root->removeChild($dom->getElementById('foo'));

var_dump($dom->getElementById('foo')?->nodeName);
?>
--EXPECT--
string(2) "p2"
