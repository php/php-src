--TEST--
Consistency of parent node after exception in ParentNode::append()
--EXTENSIONS--
dom
--FILE--
<?php
$dom = DOM\XMLDocument::createEmpty();
$fragment = $dom->createDocumentFragment();
$fragment->append($element = $dom->createElement("foo"));
$fragment->append($dom->createElement("bar"));
try {
    $dom->append($fragment);
} catch (DOMException $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

var_dump($element->parentNode);
?>
--EXPECT--
Exception: Cannot have more than one element child in a document
object(DOMDocumentFragment)#2 (21) {
  ["firstElementChild"]=>
  string(22) "(object value omitted)"
  ["lastElementChild"]=>
  string(22) "(object value omitted)"
  ["childElementCount"]=>
  int(2)
  ["nodeName"]=>
  string(18) "#document-fragment"
  ["nodeValue"]=>
  NULL
  ["nodeType"]=>
  int(11)
  ["parentNode"]=>
  NULL
  ["parentElement"]=>
  NULL
  ["childNodes"]=>
  string(22) "(object value omitted)"
  ["firstChild"]=>
  string(22) "(object value omitted)"
  ["lastChild"]=>
  string(22) "(object value omitted)"
  ["previousSibling"]=>
  NULL
  ["nextSibling"]=>
  NULL
  ["attributes"]=>
  NULL
  ["isConnected"]=>
  bool(false)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["namespaceURI"]=>
  NULL
  ["prefix"]=>
  string(0) ""
  ["localName"]=>
  NULL
  ["baseURI"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
