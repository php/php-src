--TEST--
hash_copy() errors
--FILE--
<?php

$r = hash_init("md5");
var_dump(hash_copy());
var_dump(hash_copy($r));
var_dump(hash_copy($r, $r));

echo "Done\n";
?>
--EXPECTF--	
Warning: hash_copy() expects exactly 1 parameter, 0 given in %s on line %d
NULL
resource(%d) of type (Hash Context)

Warning: hash_copy() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
