--TEST--
Bug #79836 ($a .= $a should coerce to string)
--FILE--
<?php
$a = false;
$a .= $a;
var_dump($a);
?>
--EXPECT--
string(0) ""
