--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - swapping variation
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument;
$dom->loadXML(<<<XML
<root>
    <test1 xml:id="x"/>
    <test2 attr="x"/>
</root>
XML);

$test1 = $dom->getElementById('x');

echo "--- After parsing ---\n";
var_dump($dom->getElementById('x')?->nodeName);
echo "--- After adding a new id attribute ---\n";
$dom->getElementsByTagName('test2')[0]->setIdAttribute('attr', true);
var_dump($dom->getElementById('x')?->nodeName);
echo "--- After removing the first id ---\n";
$dom->getElementById('x')->remove();
var_dump($dom->getElementById('x')?->nodeName);
echo "--- After removing the second id ---\n";
$dom->getElementById('x')->remove();
var_dump($dom->getElementById('x')?->nodeName);
echo "--- After re-adding the first id ---\n";
$dom->documentElement->appendChild($test1);
var_dump($dom->getElementById('x')?->nodeName);
echo "--- After changing the first id ---\n";
$test1->setAttribute('xml:id', 'y');
var_dump($dom->getElementById('x')?->nodeName);
?>
--EXPECT--
--- After parsing ---
string(5) "test1"
--- After adding a new id attribute ---
string(5) "test1"
--- After removing the first id ---
string(5) "test2"
--- After removing the second id ---
NULL
--- After re-adding the first id ---
string(5) "test1"
--- After changing the first id ---
NULL
