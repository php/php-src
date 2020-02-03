--TEST--
SPL: FixedArray: Testing setSize() with NULL
--FILE--
<?php

$a = new SplFixedArray(100);

$a->setSize(null);

print "ok\n";

?>
--EXPECT--
ok
