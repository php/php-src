--TEST--
DOMNode::C14N()
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<< XML
<?xml version="1.0" ?>
<books>
 <book>
  <title>The Grapes of Wrath</title>
  <author>John Steinbeck</author>
 </book>
 <book>
  <title>The Pearl</title>
  <author>John Steinbeck</author>
 </book>
</books>
XML;

$doc = new DOMDocument();
$doc->loadXML($xml);
$node = $doc->getElementsByTagName('title')->item(0);
var_dump($node->C14N());

try {
    var_dump($node->C14N(false, false, []));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($node->C14N(false, false, ['query' => []]));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
string(34) "<title>The Grapes of Wrath</title>"
DOMNode::C14N(): Argument #3 ($xpath) must have a "query" key
DOMNode::C14N(): Argument #3 ($xpath) "query" option must be a string, array given
