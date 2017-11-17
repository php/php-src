--TEST--
list with by-reference assignment should fail
--FILE--
<?php

$a = [1];
[&$foo] = $a;
$foo = 2;

var_dump($a);

?>
--EXPECTF--
Fatal error: [] and list() assignments cannot be by reference in %s on line %d
