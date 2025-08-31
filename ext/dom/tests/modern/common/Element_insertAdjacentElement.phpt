--TEST--
Element::insertAdjacentElement()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<?xml version="1.0"?><container><p>foo</p></container>');
$container = $dom->documentElement;
$p = $container->firstElementChild;

var_dump($p->insertAdjacentElement(Dom\AdjacentPosition::BeforeBegin, $dom->createElement('A'))->tagName);
echo $dom->saveXML(), "\n";

var_dump($p->insertAdjacentElement(Dom\AdjacentPosition::AfterBegin, $dom->createElement('B'))->tagName);
echo $dom->saveXML(), "\n";

var_dump($p->insertAdjacentElement(Dom\AdjacentPosition::BeforeEnd, $dom->createElement('C'))->tagName);
echo $dom->saveXML(), "\n";

var_dump($p->insertAdjacentElement(Dom\AdjacentPosition::AfterEnd, $dom->createElement('D'))->tagName);
echo $dom->saveXML(), "\n";

?>
--EXPECT--
string(1) "A"
<?xml version="1.0" encoding="UTF-8"?>
<container><A/><p>foo</p></container>
string(1) "B"
<?xml version="1.0" encoding="UTF-8"?>
<container><A/><p><B/>foo</p></container>
string(1) "C"
<?xml version="1.0" encoding="UTF-8"?>
<container><A/><p><B/>foo<C/></p></container>
string(1) "D"
<?xml version="1.0" encoding="UTF-8"?>
<container><A/><p><B/>foo<C/></p><D/></container>
