--TEST--
tidyNode::__construct()
--SKIPIF--
<?php
  if (!extension_loaded('tidy')) die ('skip tidy not present');
?>
--FILE--
<?php
new tidyNode;
?>
--EXPECTF--
Fatal error: Uncaught Error: Private method tidyNode::__construct() cannot be called from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
