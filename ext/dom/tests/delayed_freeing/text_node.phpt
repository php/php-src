--TEST--
Delayed freeing text node
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
// Text nodes are special as the underlying library has a tendency to merge adjacent ones
$text1 = $doc->appendChild($doc->createElement('container'))
    ->appendChild($doc->createTextNode('my text 1'));
$text2 = $doc->documentElement->appendChild($doc->createTextNode('my text 2'));
echo $doc->saveXML(), "\n";
$text1->parentNode->remove();
echo $doc->saveXML(), "\n";
echo $doc->saveXML($text1), "\n";
echo $doc->saveXML($text2), "\n";
var_dump($text1->parentNode, $text2->parentNode);
var_dump($text1->nextSibling, $text2->previousSibling);
?>
--EXPECT--
<?xml version="1.0"?>
<container>my text 1my text 2</container>

<?xml version="1.0"?>

my text 1
my text 2
NULL
NULL
NULL
NULL
