--TEST--
Bug #70644: trivial hash complexity DoS attack (json_decode)
--FILE--
<?php

$s = 1 << 16;
$a = [];
for ($i = 0, $j = 0; $i < $s; $i++, $j += $s) {
    $a[] = '"' . $j  . '": null';
}
$j = '{' . implode(', ', $a) . '}';
var_dump(json_decode($j, true));

?>
--EXPECTF--
Fatal error: Too many collisions in hashtable in %s on line %d
