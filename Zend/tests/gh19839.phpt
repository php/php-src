--TEST--
GH-19839: Incorrect HASH_FLAG_HAS_EMPTY_IND flag on userland array
--FILE--
<?php

const X = 'x';

$x = null;
unset(${X});

$a = $GLOBALS;
sort($a);
serialize($a);

?>
===DONE===
--EXPECT--
===DONE===
