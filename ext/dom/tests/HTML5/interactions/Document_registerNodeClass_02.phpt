--TEST--
DOM\HTML5Document::registerNodeClass 02
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->registerNodeClass("DOM\\HTML5Document", "DOMDocument");

?>
--EXPECTF--
Fatal error: Uncaught Error: DOM\Document::registerNodeClass(): Argument #2 ($extendedClass) must be a class name derived from DOM\HTML5Document or null, DOMDocument given in %s:%d
Stack trace:
#0 %s(%d): DOM\Document->registerNodeClass('DOM\\HTML5Docume...', 'DOMDocument')
#1 {main}
  thrown in %s on line %d
