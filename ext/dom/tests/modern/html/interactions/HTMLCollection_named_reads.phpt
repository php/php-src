--TEST--
HTMLCollection::namedItem() and dimension handling for named accesses
--EXTENSIONS--
dom
--FILE--
<?php

$xml = <<<XML
<!DOCTYPE root [
    <!ENTITY ent "test">
]>
<root>
    <node name="foo">1</node>
    <x id="foo">2</x>
    <x id="foo&ent;">2 with entity</x>
    <node test:id="foo" xmlns:test="http://example.com">3</node>
    <node id="wrong">4</node>
    <node id="foo">5</node>
    <node name="bar">without html ns</node>
    <node name="bar" xmlns="http://www.w3.org/1999/xhtml">with html ns</node>
</root>
XML;

$dom = DOM\XMLDocument::createFromString($xml);
var_dump($dom->getElementsByTagName('node')->namedItem('foo')?->textContent);
var_dump($dom->getElementsByTagName('node')->namedItem('')?->textContent);
var_dump($dom->getElementsByTagName('node')->namedItem('does not exist')?->textContent);
var_dump($dom->getElementsByTagName('node')->namedItem('wrong')?->textContent);
var_dump($dom->getElementsByTagName('node')->namedItem('bar')?->textContent);
var_dump($dom->getElementsByTagName('x')->namedItem('foo')?->textContent);
var_dump($dom->getElementsByTagName('x')->namedItem('footest')?->textContent);

?>
--EXPECT--
string(1) "5"
NULL
NULL
string(1) "4"
string(12) "with html ns"
string(1) "2"
string(13) "2 with entity"
