--TEST--
__compareTo: Supported by array functions with equality semantics (searching)
--FILE--
<?php
include("Comparable.inc");

$a = new Comparable(1);
$b = new Comparable(2);
$c = new Comparable(3);

$array = [$a, $b, $c];

var_dump(in_array(new Comparable(1), $array)); // yes
var_dump(in_array(new Comparable(2), $array)); // yes
var_dump(in_array(new Comparable(3), $array)); // yes

var_dump(in_array(new Comparable(0), $array)); // no
var_dump(in_array(new Comparable(9), $array)); // no

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
