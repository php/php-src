--TEST--
GH-12616 (DOM: Removing XMLNS namespace node results in invalid default: prefix)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML(
    <<<XML
    <container>
      <child1 xmlns:x="http://symfony.com/schema/dic/services">
        <x:foo x:bar=""/>
        <x:foo x:bar=""/>
      </child1>
      <child2 xmlns:x="http://symfony.com/schema/dic/services">
        <x:foo x:bar=""/>
        <x:foo x:bar=""/>
      </child2>
    </container>
    XML
);

$doc->documentElement->firstElementChild->removeAttributeNS('http://symfony.com/schema/dic/services', 'x');
echo $doc->saveXML();

$xpath = new DOMXPath($doc);

echo "--- Namespaces of child1 ---\n";

foreach ($xpath->query("/container/child1/namespace::*") as $ns) {
    var_dump($ns);
}

echo "--- Namespaces of child1/foo (both nodes) ---\n";

foreach ($xpath->query("/container/child1/foo/namespace::*") as $ns) {
    var_dump($ns);
}

echo "--- Namespaces of child2 ---\n";

foreach ($xpath->query("/container/child2/namespace::*") as $ns) {
    var_dump($ns);
}

?>
--EXPECT--
<?xml version="1.0"?>
<container>
  <child1>
    <foo bar=""/>
    <foo bar=""/>
  </child1>
  <child2 xmlns:x="http://symfony.com/schema/dic/services">
    <x:foo x:bar=""/>
    <x:foo x:bar=""/>
  </child2>
</container>
--- Namespaces of child1 ---
object(DOMNameSpaceNode)#4 (10) {
  ["nodeName"]=>
  string(9) "xmlns:xml"
  ["nodeValue"]=>
  string(36) "http://www.w3.org/XML/1998/namespace"
  ["nodeType"]=>
  int(18)
  ["prefix"]=>
  string(3) "xml"
  ["localName"]=>
  string(3) "xml"
  ["namespaceURI"]=>
  string(36) "http://www.w3.org/XML/1998/namespace"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["parentElement"]=>
  string(22) "(object value omitted)"
}
--- Namespaces of child1/foo (both nodes) ---
object(DOMNameSpaceNode)#5 (10) {
  ["nodeName"]=>
  string(9) "xmlns:xml"
  ["nodeValue"]=>
  string(36) "http://www.w3.org/XML/1998/namespace"
  ["nodeType"]=>
  int(18)
  ["prefix"]=>
  string(3) "xml"
  ["localName"]=>
  string(3) "xml"
  ["namespaceURI"]=>
  string(36) "http://www.w3.org/XML/1998/namespace"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["parentElement"]=>
  string(22) "(object value omitted)"
}
object(DOMNameSpaceNode)#8 (10) {
  ["nodeName"]=>
  string(9) "xmlns:xml"
  ["nodeValue"]=>
  string(36) "http://www.w3.org/XML/1998/namespace"
  ["nodeType"]=>
  int(18)
  ["prefix"]=>
  string(3) "xml"
  ["localName"]=>
  string(3) "xml"
  ["namespaceURI"]=>
  string(36) "http://www.w3.org/XML/1998/namespace"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["parentElement"]=>
  string(22) "(object value omitted)"
}
--- Namespaces of child2 ---
object(DOMNameSpaceNode)#9 (10) {
  ["nodeName"]=>
  string(9) "xmlns:xml"
  ["nodeValue"]=>
  string(36) "http://www.w3.org/XML/1998/namespace"
  ["nodeType"]=>
  int(18)
  ["prefix"]=>
  string(3) "xml"
  ["localName"]=>
  string(3) "xml"
  ["namespaceURI"]=>
  string(36) "http://www.w3.org/XML/1998/namespace"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["parentElement"]=>
  string(22) "(object value omitted)"
}
object(DOMNameSpaceNode)#5 (10) {
  ["nodeName"]=>
  string(7) "xmlns:x"
  ["nodeValue"]=>
  string(38) "http://symfony.com/schema/dic/services"
  ["nodeType"]=>
  int(18)
  ["prefix"]=>
  string(1) "x"
  ["localName"]=>
  string(1) "x"
  ["namespaceURI"]=>
  string(38) "http://symfony.com/schema/dic/services"
  ["isConnected"]=>
  bool(true)
  ["ownerDocument"]=>
  string(22) "(object value omitted)"
  ["parentNode"]=>
  string(22) "(object value omitted)"
  ["parentElement"]=>
  string(22) "(object value omitted)"
}
