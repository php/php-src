--TEST--
Array variable shorthand cannot mix keyed and unkeyed destructuring in list()
--FILE--
<?php
$array = [0 => 26, 'name' => 'Weilin'];
list(:$name, $age) = $array;
?>
--EXPECTF--
Fatal error: Cannot mix keyed and unkeyed array entries in assignments in %s on line %d
