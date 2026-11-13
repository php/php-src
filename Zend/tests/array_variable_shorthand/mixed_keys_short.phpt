--TEST--
Array variable shorthand cannot mix keyed and unkeyed destructuring in []
--FILE--
<?php
$array = [0 => 26, 'name' => 'Weilin'];
[:$name, $age] = $array;
?>
--EXPECTF--
Fatal error: Cannot mix keyed and unkeyed array entries in assignments in %s on line %d
