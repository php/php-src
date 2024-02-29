--TEST--
mt_srand(): Test unknown modes
--FILE--
<?php
// MT_RAND_MT19937
mt_srand(1, 0);
var_dump(mt_rand());
// MT_RAND_PHP
mt_srand(1, 1);
var_dump(mt_rand());
// Unknown
mt_srand(1, 2);
var_dump(mt_rand());
// Equivalent to 0 when cast as unsigned 8-bit integer
mt_srand(1, 256);
var_dump(mt_rand());
?>
--EXPECT--
int(895547922)
int(1244335972)
int(895547922)
int(895547922)
