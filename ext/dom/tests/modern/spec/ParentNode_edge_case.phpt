--TEST--
HierarchyRequestError on appending document to element
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$container = $dom->createElement('container');
$container->append($dom);

?>
--EXPECTF--
Fatal error: Uncaught DOMException: Hierarchy Request Error in %s:%d
Stack trace:
#0 %s(%d): DOMElement->append(Object(DOM\HTMLDocument))
#1 {main}
  thrown in %s on line %d
