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
object(DOM\DocumentFragment)#2 (17) {
  ["firstElementChild"]=>
  string(22) "(object value omitted)"
  ["lastElementChild"]=>
  string(22) "(object value omitted)"
  ["childElementCount"]=>
  int(2)
  ["nodeType"]=>
  int(11)
  ["nodeName"]=>
  string(18) "#document-fragment"
  ["baseURI"]=>
  string(11) "about:blank"
  ["isConnected"]=>
  bool(false)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
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
  ["nodeValue"]=>
  NULL
  ["textContent"]=>
  string(0) ""
}
