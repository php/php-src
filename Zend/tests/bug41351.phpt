--TEST--
Bug #41351 (Invalid opcode with foreach ($a[] as $b))
--FILE--
<?php

$a = array();

foreach($a[] as $b) {
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use [] for reading in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
