--TEST--
Ensure that DOM\Node::$prefix is read-only
--EXTENSIONS--
dom
--FILE--
<?php
$dom = DOM\HTMLDocument::createEmpty();
$div = $dom->createElement('div');
$div->prefix = "foo";
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot modify readonly property DOM\Element::$prefix in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
