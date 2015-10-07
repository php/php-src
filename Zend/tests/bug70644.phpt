--TEST--
Bug #70644: trivial hash complexity DoS attack (plain array)
--FILE--
<?php

$a = [];
$s = 1 << 16;
for ($i = 0; count($a) < $s; $i += $s) {
    $a[$i] = 0;
} 

?>
--EXPECTF--
Fatal error: Too many collisions in hashtable in %s on line %d
