--TEST--
Reflected attribute writes keep attribute children with live wrappers
--EXTENSIONS--
dom
--FILE--
<?php

function check(string $label, $doc, Closure $write): void {
    $element = $doc->documentElement;
    $text = $element->getAttributeNode('class')->firstChild;

    $write($element);

    echo $label, ": detached: ";
    var_dump($text->parentNode === null);
    echo $label, ": text: ";
    var_dump($text->textContent);
    echo $label, ": new value: ";
    var_dump($element->getAttribute('class'));
}

$doc = new DOMDocument();
$doc->loadXML('<root class="old"/>');
check('legacy className', $doc, static fn ($element) => $element->className = 'new');

$doc = Dom\XMLDocument::createFromString('<root class="old"/>');
check('modern className', $doc, static fn ($element) => $element->className = 'new');

$doc = Dom\XMLDocument::createFromString('<root class="old"/>');
check('classList value', $doc, static fn ($element) => $element->classList->value = 'new');

$doc = Dom\XMLDocument::createFromString('<root class="old"/>');
check('classList update', $doc, static fn ($element) => $element->classList->add('new'));

$doc = Dom\XMLDocument::createFromString('<root class="old"/>');
$element = $doc->documentElement;
$element->id = 'old';
$text = $element->getAttributeNode('id')->firstChild;
$element->id = 'new';
echo "id: detached: ";
var_dump($text->parentNode === null);
echo "id: text: ";
var_dump($text->textContent);
echo "id: new value: ";
var_dump($element->id);

?>
--EXPECT--
legacy className: detached: bool(true)
legacy className: text: string(3) "old"
legacy className: new value: string(3) "new"
modern className: detached: bool(true)
modern className: text: string(3) "old"
modern className: new value: string(3) "new"
classList value: detached: bool(true)
classList value: text: string(3) "old"
classList value: new value: string(3) "new"
classList update: detached: bool(true)
classList update: text: string(3) "old"
classList update: new value: string(7) "old new"
id: detached: bool(true)
id: text: string(3) "old"
id: new value: string(3) "new"
