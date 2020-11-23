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
Fatal error: Uncaught Error: Call to private tidyNode::__construct() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
