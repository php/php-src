--TEST--
Bug #79852: count(DOMNodeList) doesn't match count(IteratorIterator(DOMNodeList))
--EXTENSIONS--
dom
--FILE--
<?php

$XML = <<< XML
<root>
  <item>1</item>
  <item>2</item>
  <item>3</item>
</root>
XML;

$dom = new DomDocument();
$dom->loadXml($XML);
$items = $dom->getElementsByTagName('item');

echo "Count: ".count($items)."\n";
echo "Count: ".iterator_count($items->getIterator())."\n";
$it = new IteratorIterator($items);
echo "Count: ".iterator_count($it)."\n";
echo "Count: ".iterator_count($it)."\n";

?>
--EXPECTF--
Count: 3
Count: 3
Count: 3

Fatal error: Uncaught Error: Iterator does not support rewinding in %s:%d
Stack trace:
#0 [internal function]: InternalIterator->rewind()
#1 [internal function]: IteratorIterator->rewind()
#2 %s(%d): iterator_count(Object(IteratorIterator))
#3 {main}
  thrown in %s on line %d
