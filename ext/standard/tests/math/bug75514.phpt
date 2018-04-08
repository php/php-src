--TEST--
Bug #75514 mt_rand returns value outside [$min,$max]
--FILE--
<?php
mt_srand(0, MT_RAND_PHP);
var_dump(mt_rand(0,999999999), mt_rand(0,999));
?>
===Done===
--EXPECT--
int(448865905)
int(592)
===Done===
