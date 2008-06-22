--TEST--
SPL: FixedArray: Setting size
--FILE--
<?php

$a = new SplFixedArray(0);
$a = new SplFixedArray(3);

$a[0] = 1;

$a->setSize(2);
$a->setSize(3);
$a->setSize(0);

print "ok\n";

?>
--EXPECT--
ok
