--TEST--
SPL: FastArray: Setting size to 0
--FILE--
<?php

$a = new SplFastArray(1);

$a[0] = 1;

$a->setSize(0);

print "ok\n";

?>
--EXPECT--
ok
