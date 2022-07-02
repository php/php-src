--TEST--
DOMParentNode::prepend()
--EXTENSIONS--
dom
--FILE--
<?php
declare(strict_types=1);

require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML('<test><mark/><mark><nested /></mark><mark/></test>');

$element = $dom->documentElement;
$firstMark = $element->childNodes[0];
$element->prepend(
  $dom->createElement('element', 'content'),
  'content'
);

var_dump($element->childElementCount);
print_node_list_compact($element->childNodes);

$element = $dom->documentElement;
$element->prepend(
  $dom->createElement('element', 'content'),
  'content'
);
var_dump($element->childElementCount);

$firstMark->prepend('content');
print_node_list_compact($firstMark->childNodes);
?>
--EXPECT--
int(4)
<element>
  content
</element>
content
<mark>
</mark>
<mark>
  <nested>
  </nested>
</mark>
<mark>
</mark>
int(5)
content
