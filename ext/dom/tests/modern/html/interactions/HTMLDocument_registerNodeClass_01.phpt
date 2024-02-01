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
Fatal error: Uncaught Error: DOM\Document::registerNodeClass(): Argument #2 ($extendedClass) must be a class name derived from DOM\HTMLDocument or null, DOMDocument given in %s:%d
Stack trace:
#0 %s(%d): DOM\Document->registerNodeClass('DOM\\HTMLDocumen...', 'DOMDocument')
#1 {main}
  thrown in %s on line %d
