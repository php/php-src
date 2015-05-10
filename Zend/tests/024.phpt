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
Notice: Undefined variable: a in %s on line 3

Notice: Trying to get index of a non-array in %s on line 3
NULL

Notice: Undefined variable: a in %s on line 4

Notice: Undefined variable: c in %s on line 4

Notice: Trying to get index of a non-array in %s on line 4
NULL

Notice: Undefined variable: a in %s on line 5
int(1)

Notice: Undefined variable: a in %s on line 6

Notice: Undefined variable: b in %s on line 6
int(0)

Notice: Undefined variable: a in %s on line 7
NULL

Notice: Undefined variable: b in %s on line 8
int(1)

Notice: Trying to get property of non-object in %s on line 9
NULL

Notice: Trying to get property of non-object in %s on line 10
NULL

Notice: Undefined variable: c in %s on line 11

Notice: Trying to get index of a non-array in %s on line 11

Notice: Trying to get property of non-object in %s on line 11

Notice: Trying to get property of non-object in %s on line 11
NULL
