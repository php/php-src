--TEST--
Bug #41351 (Invalid opcode with foreach ($a[] as $b)) - 3
--FILE--
<?php

$a = array();

foreach($a['test'][] as $b) {
}

echo "Done\n";
?>
--EXPECTF--
Warning: Undefined array key "test" in %s on line %d

Fatal error: Uncaught Error: Cannot use [] for reading in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
