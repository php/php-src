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
var_dump(json_last_error() == JSON_ERROR_COLLISIONS);

?>
--EXPECTF--
NULL
bool(true)
