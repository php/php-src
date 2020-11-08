--TEST--
Testing isset accessing undefined array items and properties
--FILE--
<?php

$a = 'foo';
$b =& $a;

var_dump(isset($b));

var_dump(isset($a[0], $b[1]));

var_dump(isset($a[0]->a));

var_dump(isset($c[0][1][2]->a->b->c->d));

var_dump(isset(${$a}->{$b->{$c[$d]}}));

var_dump(isset($GLOBALS));

var_dump(isset($GLOBALS[1]));

var_dump(isset($GLOBALS[1]->$GLOBALS));

?>
--EXPECTF--
bool(true)
bool(true)
bool(false)
bool(false)

Warning: Undefined variable $c in %s on line %d

Warning: Undefined variable $d in %s on line %d

Warning: Trying to access array offset on value of type null in %s on line %d

Warning: Attempt to read property "" on string in %s on line %d
bool(false)
bool(true)
bool(false)
bool(false)
