--TEST--
Unpacking vs foreach behaviour
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML('<root><a/><b/></root>');

echo "--- By foreach: ---\n";

foreach ($dom->documentElement->getElementsByTagName('*') as $node) {
    var_dump($node->localName);
}

echo "--- By unpacking: ---\n";

$iter = $dom->documentElement->getElementsByTagName('*');
foreach ([...$iter] as $node) {
    var_dump($node->localName);
}

?>
--EXPECT--
--- By foreach: ---
string(1) "a"
string(1) "b"
--- By unpacking: ---
string(1) "a"
string(1) "b"
