--TEST--
Testing operations with undefined variable
--FILE--
<?php

var_dump($a[1]);
var_dump($a[$c]);
var_dump($a + 1);
var_dump($a + $b);
var_dump($a++);
var_dump(++$b);
var_dump($a->$b);
var_dump($a->$b);
var_dump($a->$b->{$c[1]});

?>
--EXPECTF--
Warning: Undefined variable: a in %s on line %d

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Undefined variable: a in %s on line %d

Warning: Undefined variable: c in %s on line %d

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Undefined variable: a in %s on line %d
int(1)

Warning: Undefined variable: a in %s on line %d

Warning: Undefined variable: b in %s on line %d
int(0)

Warning: Undefined variable: a in %s on line %d
NULL

Warning: Undefined variable: b in %s on line %d
int(1)

Warning: Trying to get property '1' of non-object in %s on line %d
NULL

Warning: Trying to get property '1' of non-object in %s on line %d
NULL

Warning: Undefined variable: c in %s on line %d

Warning: Trying to access array offset on value of type null in %s on line %d

Warning: Trying to get property '1' of non-object in %s on line %d

Warning: Trying to get property '' of non-object in %s on line %d
NULL
