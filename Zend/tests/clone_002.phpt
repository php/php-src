--TEST--
Testing multiple clone statements
--FILE--
<?php 

$a = clone clone $b = new stdClass;
var_dump($a == $b);


$c = clone clone clone $b = new stdClass;
var_dump($a == $b, $b == $c);

class foo { }

$d = clone $a = $b = new foo;
var_dump($a == $d, $b == $d, $c == $a);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
