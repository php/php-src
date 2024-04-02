--TEST--
DOM\HTMLDocument::registerNodeClass 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->registerNodeClass("DOM\\HTMLDocument", "DOMDocument");

?>
--EXPECTF--
Fatal error: Uncaught TypeError: DOMDocument::registerNodeClass(): Argument #1 ($baseClass) must be a class name derived from DOMNode, DOM\HTMLDocument given in %s:%d
Stack trace:
#0 %s(%d): DOMDocument->registerNodeClass('DOM\\HTMLDocumen...', 'DOMDocument')
#1 {main}
  thrown in %s on line %d
