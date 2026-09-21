--TEST--
GHSA-x692-q9x7-8c3f: bccomp() out-of-bounds write
--CREDITS--
Recep Asan (recepasan)
--FILE--
<?php

$n = '1.' . '9' . str_repeat('0', 300) . '1';
var_dump(bccomp($n, '0', 300));

?>
--EXPECT--
int(1)
