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
Notice: Undefined variable: a in %s on line %d

Notice: Trying to access array offset on value of type null in %s on line %d
NULL

Notice: Undefined variable: a in %s on line %d

Notice: Undefined variable: c in %s on line %d

Notice: Trying to access array offset on value of type null in %s on line %d
NULL

Notice: Undefined variable: a in %s on line %d
int(1)

Notice: Undefined variable: a in %s on line %d

Notice: Undefined variable: b in %s on line %d
int(0)

Notice: Undefined variable: a in %s on line %d
NULL

Notice: Undefined variable: b in %s on line %d
int(1)

Notice: Trying to get property '1' of non-object in %s on line %d
NULL

Notice: Trying to get property '1' of non-object in %s on line %d
NULL

Notice: Undefined variable: c in %s on line %d

Notice: Trying to access array offset on value of type null in %s on line %d

Notice: Trying to get property '1' of non-object in %s on line %d

Notice: Trying to get property '' of non-object in %s on line %d
NULL
