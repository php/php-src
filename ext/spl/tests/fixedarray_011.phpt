--TEST--
SPL: FixedArray: Testing setSize() with NULL
--FILE--
<?php

$a = new SplFixedArray(100);

$a->setSize(NULL);

print "ok\n";

?>
--EXPECT--
ok
