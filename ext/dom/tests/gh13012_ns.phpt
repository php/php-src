--TEST--
GH-13012 (DOMNode::isEqualNode() is incorrect when attribute order is different - ns variation)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML("<root><x/><x xmlns:a=\"urn:a\" xmlns:b=\"urn:b\"/><x xmlns:b=\"urn:b\" xmlns:a=\"urn:a\"/></root>");

foreach ($dom->getElementsByTagName('x') as $x1) {
    foreach ($dom->getElementsByTagName('x') as $x2) {
        echo "Comparing ", $dom->saveXML($x1), " with ", $dom->saveXML($x2), "\n";
        var_dump($x1->isEqualNode($x2));
    }
}

?>
--EXPECT--
Comparing <x/> with <x/>
bool(true)
Comparing <x/> with <x xmlns:a="urn:a" xmlns:b="urn:b"/>
bool(false)
Comparing <x/> with <x xmlns:b="urn:b" xmlns:a="urn:a"/>
bool(false)
Comparing <x xmlns:a="urn:a" xmlns:b="urn:b"/> with <x/>
bool(false)
Comparing <x xmlns:a="urn:a" xmlns:b="urn:b"/> with <x xmlns:a="urn:a" xmlns:b="urn:b"/>
bool(true)
Comparing <x xmlns:a="urn:a" xmlns:b="urn:b"/> with <x xmlns:b="urn:b" xmlns:a="urn:a"/>
bool(true)
Comparing <x xmlns:b="urn:b" xmlns:a="urn:a"/> with <x/>
bool(false)
Comparing <x xmlns:b="urn:b" xmlns:a="urn:a"/> with <x xmlns:a="urn:a" xmlns:b="urn:b"/>
bool(true)
Comparing <x xmlns:b="urn:b" xmlns:a="urn:a"/> with <x xmlns:b="urn:b" xmlns:a="urn:a"/>
bool(true)
