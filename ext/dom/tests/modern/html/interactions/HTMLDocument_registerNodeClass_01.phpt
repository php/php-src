--TEST--
Dom\HTMLDocument::registerNodeClass 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->registerNodeClass("Dom\\HTMLDocument", "DOMDocument");

?>
--EXPECTF--
Fatal error: Uncaught TypeError: DOMDocument::registerNodeClass(): Argument #1 ($baseClass) must be a class name derived from DOMNode, Dom\HTMLDocument given in %s:%d
Stack trace:
#0 %s(%d): DOMDocument->registerNodeClass('Dom\\HTMLDocumen...', 'DOMDocument')
#1 {main}
  thrown in %s on line %d
