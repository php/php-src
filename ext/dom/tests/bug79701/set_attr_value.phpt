--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - nodeValue / value variation
--EXTENSIONS--
dom
--FILE--
<?php
foreach (["value", "nodeValue"] as $property) {
    echo "--- Testing property \$$property ---\n";
    $dom = Dom\XMLDocument::createFromString(<<<XML
    <root>
        <test1 xml:id="x"/>
        <test2 xml:id="y"/>
    </root>
    XML);

    $test1 = $dom->getElementById('x');
    $test2 = $dom->getElementById('y');

    var_dump($test1?->nodeName);
    var_dump($test2?->nodeName);

    $test1->getAttributeNode('xml:id')->$property = "y";
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    $test2->getAttributeNode('xml:id')->$property = "x";
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);
}
?>
--EXPECT--
--- Testing property $value ---
string(5) "test1"
string(5) "test2"
NULL
string(5) "test1"
string(5) "test2"
string(5) "test1"
--- Testing property $nodeValue ---
string(5) "test1"
string(5) "test2"
NULL
string(5) "test1"
string(5) "test2"
string(5) "test1"
