--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - remove attribute variation
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\XMLDocument::createFromString(<<<XML
<root>
    <test1 xml:id="x"/>
    <test2 xml:id="x"/>
</root>
XML);

var_dump($dom->getElementById('x')?->nodeName);
$dom->getElementById('x')->removeAttribute('xml:id');
var_dump($dom->getElementById('x')?->nodeName);
?>
--EXPECTF--
Warning: Dom\XMLDocument::createFromString(): ID x already defined in Entity, line: 3 in %s on line %d
string(5) "test1"
NULL
