--TEST--
SPL: FastArray: Testing setSize() with NULL
--FILE--
<?php

$a = new SplFastArray(100);

$a->setSize(NULL);

print "ok\n";

?>
--EXPECT--
ok
