--TEST--
GH-13012 (DOMNode::isEqualNode() is incorrect when attribute order is different)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML("<root><x a='a' b='b'/><x b='b' a='a'/><x b='b' a='a' c='c'/></root>");

foreach ($dom->getElementsByTagName('x') as $x1) {
    foreach ($dom->getElementsByTagName('x') as $x2) {
        echo "Comparing ", $dom->saveXML($x1), " with ", $dom->saveXML($x2), "\n";
        var_dump($x1->isEqualNode($x2));
    }
}

?>
--EXPECT--
Comparing <x a="a" b="b"/> with <x a="a" b="b"/>
bool(true)
Comparing <x a="a" b="b"/> with <x b="b" a="a"/>
bool(true)
Comparing <x a="a" b="b"/> with <x b="b" a="a" c="c"/>
bool(false)
Comparing <x b="b" a="a"/> with <x a="a" b="b"/>
bool(true)
Comparing <x b="b" a="a"/> with <x b="b" a="a"/>
bool(true)
Comparing <x b="b" a="a"/> with <x b="b" a="a" c="c"/>
bool(false)
Comparing <x b="b" a="a" c="c"/> with <x a="a" b="b"/>
bool(false)
Comparing <x b="b" a="a" c="c"/> with <x b="b" a="a"/>
bool(false)
Comparing <x b="b" a="a" c="c"/> with <x b="b" a="a" c="c"/>
bool(true)
