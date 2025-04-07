--TEST--
CSS Selectors - Handling entities
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString(<<<XML
<!DOCTYPE root [
    <!ENTITY ent "box">
]>
<root>
    <input type="check&ent;" checked="" class="a&ent;a" id="a&ent;a" xmlns="http://www.w3.org/1999/xhtml"/>
</root>
XML);
var_dump($dom->querySelector('input:checked')->nodeName);
var_dump($dom->querySelector('input[type$="ox"]')->nodeName);
var_dump($dom->querySelector('input.aboxa')->nodeName);
var_dump($dom->querySelector('input#aboxa')->nodeName);

?>
--EXPECT--
string(5) "input"
string(5) "input"
string(5) "input"
string(5) "input"
