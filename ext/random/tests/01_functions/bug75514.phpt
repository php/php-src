--TEST--
Bug #75514 mt_rand returns value outside [$min,$max]
--FILE--
<?php
mt_srand(0, MT_RAND_PHP);
var_dump(mt_rand(0,999999999), mt_rand(0,999));
?>
--EXPECTF--
Deprecated: Constant MT_RAND_PHP is deprecated in %s on line %d

Deprecated: The MT_RAND_PHP variant of Mt19937 is deprecated in %s on line %d
int(448865905)
int(592)
