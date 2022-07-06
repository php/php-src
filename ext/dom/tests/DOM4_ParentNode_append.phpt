--TEST--
DOMParentNode::append()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
declare(strict_types=1);

require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test><mark/><mark /><mark /></test>');

$element = $dom->documentElement;
$element->append(
  $dom->createElement('element', 'content'),
  'content'
);

var_dump($dom->documentElement->childElementCount);
print_node_list_compact($element->childNodes);

$element->append(
  $dom->createElement('element', 'content'),
  'content'
);
var_dump($dom->documentElement->childElementCount);
?>
--EXPECT--
int(4)
<mark>
</mark>
<mark>
</mark>
<mark>
</mark>
<element>
  content
</element>
content
int(5)
